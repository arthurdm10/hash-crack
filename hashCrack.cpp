
#include "stdafx.h"
#include <thread>

#include "ArgParser.h"
#include "crack.h"


int main(int argc, char **argv) {

	using HashMap = std::unordered_map<std::string, std::pair<uint32_t, HashFunction>>;

	auto argParser = ArgParser(fmt::format("usage: {} --hash YOUR-HASH --hash-type md5 --wordlist path/to/wordlist.txt", argv[0]));

	argParser.addArg("hash", "Hash to be cracked", true);
	argParser.addArg("wordlist", "Wordlist path", true);
	argParser.addArg("hash-type", "MD5, SHA1, SHA256, SHA512", true);


	auto totalThreads = std::thread::hardware_concurrency();
	argParser.addArg("threads", fmt::format("Total threads to run. Default is {}",
														totalThreads), false);

	argParser.parseArguments(argc, argv);


	const std::string hashToCrack = argParser.get("hash");
	std::string hashType = argParser.get("hash-type");

	const HashMap hashs = { {"md5", {32, hash<CryptoPP::Weak::MD5, CryptoPP::Weak::MD5::DIGESTSIZE>}},
							{"sha1",   {40, hash<CryptoPP::SHA1, CryptoPP::SHA1::DIGESTSIZE> } },
							{"sha256", {64, hash<CryptoPP::SHA256, CryptoPP::SHA256::DIGESTSIZE>}},
							{"sha512", {128, hash<CryptoPP::SHA512, CryptoPP::SHA512::DIGESTSIZE>}} };


	std::transform(hashType.begin(), hashType.end(), hashType.begin(), tolower);

	
	HashFunction hashFunction;

	if (hashs.find(hashType) == hashs.end()) {
		fmt::print("Invalid hash type {}\n\n", hashType);
		return 1;
	} else if (hashs.at(hashType).first != hashToCrack.size()) {
		fmt::print("Invalid {} hash", hashType);
		return 1;
	} else {
		hashFunction = hashs.at(hashType).second;
	}
	
	std::queue<std::string> q;

	std::ifstream file(argParser.get("wordlist"));

	if (!file.is_open()) {
		std::cout << "Could not open wordlist '" + argParser.get("wordlist") + "'\n";
		return 1;
	}


	std::vector<std::thread> threads;
	std::thread th(readStringFromFile, std::ref(q), std::ref(file));

	const auto start = std::chrono::high_resolution_clock::now();
	if (argParser.isSet("threads")) {
		if (int t = argParser.get<int>("threads"); t > 0) {
			totalThreads = t;
		}
	}


	fmt::print("\n\n[+]Threads: {}\n\
[+]Hash type: {}\n\
[+]Hash: {}\n\n", totalThreads, hashType, hashToCrack);

	for (unsigned int i = 0; i < totalThreads; i++) {
		threads.emplace_back(crack, std::ref(q),
			std::ref(hashToCrack),
			std::ref(hashFunction));
	}


	th.join();

	for (auto& thread : threads) {
		thread.join();
	}

	std::cout << "Done\n";
	if (found) {
		fmt::print("Hash found\n{} = {}\n", hashFound, hashToCrack);
	}else {
		std::cout << "Hash not found\n";
	}

	
	std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - start;

	fmt::print("It took {0:.2f} seconds", diff.count());

	return 0;
}

