#include <mysdk/base/Logging.h>

#include <mysdk/base/Timestamp.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

namespace mysdk
{
	class LoggerImpl
	{
	public:
		typedef Logger::LogLevel LogLevel;
		LoggerImpl(LogLevel level, int old_errno, const char* file, int line);
		void finish();

		Timestamp time_;
		std::ostringstream stream_;
		LogLevel level_;
		const char* fullname_;
		int line_;
		const char* basename_;
		const char* function_;

		static const char* LogLevelName[];
	};

	__thread char t_errnobuf[512];

	const char* strerror_tl(int savedErrno)
	{
		return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
	}

	Logger::LogLevel initLogLevel()
	{
		if (::getenv("MYSDK_LOG_TRACE"))
		{
			return Logger::TRACE;
		}
		else
		{
			return Logger::DEBUG;
		}
	}

	Logger::LogLevel g_logLevel = initLogLevel();
}

using namespace mysdk;

const char* LoggerImpl::LogLevelName[Logger::NUM_LOG_LEVELS] =
{
		"TRACE",
		"DEBUF",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL",
};

LoggerImpl::LoggerImpl(LogLevel level, int savedErrno, const char* file, int line):
		time_(Timestamp::now()),
		stream_(),
		level_(level),
		fullname_(file),
		line_(line),
		basename_(NULL),
		function_(NULL)
{
	const char* path_sep_pos = strrchr(fullname_, '/');
	basename_ = (path_sep_pos != NULL) ? path_sep_pos + 1 : fullname_;
	char message_head[512];
	snprintf(message_head, sizeof(message_head) - 1, "%s  %s ",
			time_.toFormattedString().c_str(), LogLevelName[level]);
	stream_ <<  message_head;
	if (savedErrno != 0)
	{
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ")";
	}
}

void LoggerImpl::finish()
{
	stream_ << "-" << basename_ << ":" << line_ << '\n';
}

std::ostream& Logger::stream()
{
	assert(pImpl_ != NULL);
	return pImpl_->stream_;
}

Logger::Logger(const char* file, int line):
		pImpl_(new LoggerImpl(INFO, 0, file, line))
{
}

Logger::Logger(const char* file, int line, LogLevel level, const char* func):
		pImpl_(new LoggerImpl(level, 0, file, line))
{
	pImpl_->stream_ << func << ' ';
}

Logger::Logger(const char* file, int line, LogLevel level):
		pImpl_(new LoggerImpl(level, 0, file, line))
{

}

Logger::Logger(const char* file, int line, bool toAbort):
		pImpl_(new LoggerImpl(toAbort ? FATAL : ERROR, errno, file, line))
{

}

Logger::~Logger()
{
	assert(pImpl_ != NULL);
	pImpl_->finish();
	std::string buf(pImpl_->stream_.str());
	ssize_t n = ::write(1, buf.data(), buf.size());
	(void)n;
	if (pImpl_->level_ == FATAL)
	{
		abort();
	}
	delete pImpl_;
}

Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}
