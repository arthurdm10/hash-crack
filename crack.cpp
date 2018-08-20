#include "stdafx.h"
#include "crack.h"

std::mutex mutex, printMutex, fileMutex;
std::condition_variable condVar;
std::atomic<bool> found;

std::string hashFound;

void crack(std::queue<std::string>& q,
	const std::string& hash,
	const HashFunction& hashFunction) {

	while (!found) {
		auto now = std::chrono::system_clock::now();

		std::unique_lock<std::mutex> lock(mutex);
		if (!condVar.wait_until(lock, now + 1s, [&q]() {return q.size() != 0; })) {
			break;
		}

		auto str = q.front();
		q.pop();
		lock.unlock();

		std::string strHash = hashFunction(str);



		if (strHash == hash) {
			found = true;
			hashFound = str;
		}
	}
}

void readStringFromFile(std::queue<std::string>& q, std::ifstream& file) {
	std::string line;

	while (std::getline(file, line) && !found) {
		std::unique_lock<std::mutex> lock(mutex);
		q.push(line);
		condVar.notify_all();
	}
	file.close();
}