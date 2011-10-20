
#ifndef MYSDK_BASE_DATA_H
#define MYSDK_BASK_DATA_H

#include <string>

#include <mysdk/base/Common.h>

using namespace std;
struct tm;

namespace mysdk
{
	class Date
	{
	public:
		  struct YearMonthDay
		  {
		    int year; // [1900..2500]
		    int month;  // [1..12]
		    int day;  // [1..31]
		  };
	public:
		  static const int kDaysPerWeek = 7;
		  Date():
			  julianDayNumber_(0)
		  {
		  }

		  Date(int year, int month, int day);
		  explicit Date(int julianDayNum):
				  julianDayNumber_(julianDayNum)
		  {
		  }

		  explicit Date(const struct tm&);

		  // default copy/assignment/dtor are Okay

		  void swap(Date& that)
		  {
		    std::swap(julianDayNumber_, that.julianDayNumber_);
		  }

		  bool valid() const { return julianDayNumber_ > 0; }

		  ///
		  /// Converts to yyyy-mm-dd format.
		  ///
		  string toIsoString() const;

		  struct YearMonthDay yearMonthDay() const;

		  int year() const
		  {
		    return yearMonthDay().year;
		  }

		  int month() const
		  {
		    return yearMonthDay().month;
		  }

		  int day() const
		  {
		    return yearMonthDay().day;
		  }

		  // [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
		  int weekDay() const
		  {
		    return (julianDayNumber_+1) % kDaysPerWeek;
		  }

		  int julianDayNumber() const { return julianDayNumber_; }
	 private:
		  int julianDayNumber_;
	};

	inline bool operator<(Date x, Date y)
	{
	  return x.julianDayNumber() < y.julianDayNumber();
	}

	inline bool operator==(Date x, Date y)
	{
	  return x.julianDayNumber() == y.julianDayNumber();
	}
}
#endif
