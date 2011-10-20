#include <mysdk/base/Timestamp.h>
#include <stdio.h>

using mysdk::Timestamp;

void passByConstReference(const Timestamp& x)
{
  printf("%s\n", x.toString().c_str());
  printf("%s\n", x.toFormattedString().c_str());
}

void passByValue(Timestamp x)
{
  printf("%s\n", x.toString().c_str());
  printf("%s\n", x.toFormattedString().c_str());
}

int main()
{
  Timestamp now(Timestamp::now());
  printf("%s\n", now.toString().c_str());
  printf("%s\n", now.toFormattedString().c_str());
  passByValue(now);
  passByConstReference(now);
}

