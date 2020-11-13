/**
 * File: trace-system-calls.h
 * --------------------------
 * Exports a collection of functions that help keep track of all system call names,
 * numbers, and argument lists.  The functions crawl over the system directories.
 */
 
#pragma once
#include <map>
#include <vector>
#include <ostream>

/**
 * Type: scParamType
 * -----------------
 * The scParamType distinguishes helps keep track of which data types
 * contribue to system call signatures.  The trace executable distinguishes between
 * integer types (SYSCALL_INTEGER), const char * and const char __user * types (SYSCALL_STRING),
 * and other pointer types (SYSCALL_POINTER).
 */
enum scParamType {
  SYSCALL_INTEGER, 
  SYSCALL_STRING, 
  SYSCALL_POINTER,
  SYSCALL_UNKNOWN_TYPE
};

/**
 * Function: operator<< for scParamType
 * ------------------------------------
 * Self-explanatory overload of the << operator for scParamType, so they
 * know how to print themselves.
 */
std::ostream& operator<<(std::ostream& os, const scParamType& type);

/**
 * Function: operator>> for scParamType
 * ------------------------------------
 * Self-explanatory overload of the >> operator for scParamType, so they know
 * how to pull themselves out of istreams.
 */
std::istream& operator>>(std::istream& is, scParamType& type);

/**
 * Type: systemCallSignature
 * -------------------------
 * Defines another name for vector<scParamType>, which is used in several
 * places to summarize the data types that should be passed to a system call.
 */
typedef std::vector<scParamType> systemCallSignature;

/**
 * Function: compileSystemCallData
 * -------------------------------
 * Initialize the three provided maps with information about system call names, their corresponding numbers,
 * and signatures.  All three parameters are expected to reference allocated but otherwise empty maps.  To illustrate, assume
 * that there are just four system calls with the following signatures:
 *
 *      int open(const char *, int flags, int mode);              // system call number 1
 *      ssize_t read(int fd, void *buffer, size_t size);          // system call number 2
 *      ssize_t write(int fd, const void *buffer, size_t size);   // system call number 3
 *      int close(int fd);                                        // system call number 4
 *
 * On return, the three maps would contain the following key/value pairs:
 *
 *       systemCallNumbers: 1 -> "open",
 *                          2 -> "read",
 *                          3 -> "write",
 *                          4 -> "close"
 *       systemCallNames: "open" -> 1,
 *                        "read" -> 2,
 *                        "write" -> 3,
 *                        "close" -> 4
 *       systemCallSignatures: "open" -> [SYSCALL_STRING, SYSCALL_INTEGER, SYSCALL_INTEGER],
 *                             "read" -> [SYSCALL_INTEGER, SYSCALL_POINTER, SYSCALL_INTEGER],
 *                             "write" -> [SYSCALL_INTEGER, SYSCALL_POINTER, SYSCALL_INTEGER],
 *                             "close" -> [SYSCALL_INTEGER]
 *
 * The rebuild boolean, if true, is an instruction to rebuild the map of prototype information from scratch
 * instead of relying on a cached data file.
 */
void compileSystemCallData(std::map<int, std::string>& systemCallNumbers,
                           std::map<std::string, int>& systemCallNames,
                           std::map<std::string, systemCallSignature>& systemCallSignatures, bool rebuild);
