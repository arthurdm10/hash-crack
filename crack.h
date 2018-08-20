#ifndef _CRACK_H
#define _CRACK_H

#include <thread>
#include <mutex>
#include <queue>
#include <iostream>
#include <string>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <fstream>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "md5.h"
#include "sha.h"
#include "hex.h"

using namespace std::chrono_literals;
using byte = unsigned char;
using HashFunction = std::function<std::string(const std::string&)>;

extern std::mutex mutex, printMutex, fileMutex;
extern std::condition_variable condVar;
extern std::atomic<bool> found;

extern std::string hashFound;


template<class _HashType, int _DIGESTSIZE>
std::string hash(const std::string& data) {

	byte digest[_DIGESTSIZE];
	_HashType hashClass;

	hashClass.CalculateDigest(digest, (byte*)data.c_str(), data.length());

	CryptoPP::HexEncoder encoder;
	std::string output;

	encoder.Attach(new CryptoPP::StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();

	return output;
}

void crack(std::queue<std::string>& q,
	const std::string& hash,
	const HashFunction& hashFunction);

void readStringFromFile(std::queue<std::string>& q, std::ifstream& file);

#endif