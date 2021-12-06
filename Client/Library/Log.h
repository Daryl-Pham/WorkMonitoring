#pragma once

#include <sal.h>  // For _Printf_format_string_
#include <stdio.h>

enum LoggerLogType
{
    LOGGER_LOG_TYPE_ERROR,
    LOGGER_LOG_TYPE_WARN,
    LOGGER_LOG_TYPE_INFO,
    LOGGER_LOG_TYPE_DEBUG,
    LOGGER_LOG_TYPE_VERBOSE,
};

#define LOGGER_FILE_NAME_MCRO (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

/* Reference code https://stackoverflow.com/a/8488201
 * define で LOGGER_FILE_NAME を定義すると LOG_* を呼び出すたびに strrchr が呼び出され、コストがかかる。
 * 定数として LOGGER_FILE_NAME を定義することで、 strrchr の呼び出し回数を減らしている。
 */
#define DEFINE_LOGGER_FILE_NAME const char *LOGGER_FILE_NAME = LOGGER_FILE_NAME_MCRO

void LoggerInitialize(LoggerLogType type, const wchar_t *directory);
void LoggerFinalize();
void LoggerLog(LoggerLogType type, int line, const char *tag, _Printf_format_string_ char const *const format, ...);

#define LOG_TAG_ERROR(tag, format, ...) LoggerLog(LOGGER_LOG_TYPE_ERROR, __LINE__, (tag), (format), __VA_ARGS__)
#define LOG_TAG_WARN(tag, format, ...) LoggerLog(LOGGER_LOG_TYPE_WARN, __LINE__, (tag), (format), __VA_ARGS__)
#define LOG_TAG_INFO(tag, format, ...) LoggerLog(LOGGER_LOG_TYPE_INFO, __LINE__, (tag), (format), __VA_ARGS__)
#define LOG_TAG_DEBUG(tag, format, ...) LoggerLog(LOGGER_LOG_TYPE_DEBUG, __LINE__, (tag), (format), __VA_ARGS__)
#define LOG_TAG_VERBOSE(tag, format, ...) LoggerLog(LOGGER_LOG_TYPE_VERBOSE, __LINE__, (tag), (format), __VA_ARGS__)

#define LOG_ERROR(format, ...) LOG_TAG_ERROR(LOGGER_FILE_NAME, (format), __VA_ARGS__)
#define LOG_WARN(format, ...) LOG_TAG_WARN(LOGGER_FILE_NAME, (format), __VA_ARGS__)
#define LOG_INFO(format, ...) LOG_TAG_INFO(LOGGER_FILE_NAME, (format), __VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_TAG_DEBUG(LOGGER_FILE_NAME, (format), __VA_ARGS__)
#define LOG_VERBOSE(format, ...) LOG_TAG_VERBOSE(LOGGER_FILE_NAME, (format), __VA_ARGS__)
