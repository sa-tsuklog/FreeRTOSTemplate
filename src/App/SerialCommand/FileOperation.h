/*
 * FileOperation.h
 *
 *  Created on: 2016/08/22
 *      Author: sa
 */

#ifndef FILEOPERATION_H_
#define FILEOPERATION_H_

class FileOperation {
	// Singleton pattern definition
private:
	FileOperation();
	FileOperation(const FileOperation& rhs);
	FileOperation& operator=(const FileOperation& rhs);
	virtual ~FileOperation() {}
public:
	static FileOperation* GetInstance() {
		static FileOperation instance;
		return &instance;
	}

	// Class definition
private:
	static const int MAX_LFN_LEN = 256;
	char lfn[MAX_LFN_LEN];
public:
	void ls();
	void cat(char* filename);
};

#endif /* FILEOPERATION_H_ */
