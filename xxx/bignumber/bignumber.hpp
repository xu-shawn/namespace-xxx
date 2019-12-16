#pragma once


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace xxx
{
	class bigNumber
	{
	public:
		~bigNumber() {}
		void resort(std::vector<short>& resortNumber)
			{
				for (auto itertor=std::begin(resortNumber);itertor!=std::end(resortNumber);++itertor)
				{
					auto x = *itertor;
					if (x > 9)
					{
						if (itertor != std::end(resortNumber))
							*(itertor+1) += x % 10;
						else
							resortNumber.push_back(x % 10);

						*itertor = x / 10;
					}
				}

			};
	public:
		std::vector<short> number;

		bigNumber()
		{
		}

		bigNumber(const std::string str)
		{
			number.assign(std::crbegin(str), std::crend(str));
			for (auto& x : number)
				x -= '0';

		}

		bigNumber operator=(const bigNumber& otherNumber)
		{
			number.assign(cbegin(otherNumber.number), cend(otherNumber.number));
		}

		void operator=(const std::string str)
		{
			number.assign(std::crbegin(str), std::crend(str));
			for (auto& x : number)
				x -= '0';
		}


		bigNumber operator+(const bigNumber& otherNumber)
		{
			bigNumber sumNumber;
			sumNumber.number.assign(std::cbegin(number), std::cend(number));

			int j = 0;
			for (auto x : otherNumber.number)
			{
				if (j < number.size())
					sumNumber.number[j] += x;
				else
					sumNumber.number.push_back(x);
				j++;
			}

			resort(sumNumber.number);
			return sumNumber;
		}

		std::string getstring()
		{
			std::string str;
			str.assign(std::crbegin(number), std::crend(number));
			for (auto& x : str)
				x += '0';
			auto positionBegin = str.find_first_not_of('0');
			str.erase(0, positionBegin);
			return str;
		}

		friend std::ostream& operator<<(std::ostream& os, const bigNumber& otherNumber)
		{
			std::string str;
			str.assign(std::crbegin(otherNumber.number), std::crend(otherNumber.number));
			for (auto& x : str)
				x += '0';
			auto positionBegin = str.find_first_not_of('0');
			str.erase(0, positionBegin);
			os<< str;
			return os;
		}

		friend std::istream& operator>>(std::istream& is, bigNumber& otherNumber)
		{
			std::string str;
			is >> str;
			otherNumber.number.assign(std::crbegin(str), std::crend(str));
			for (auto& x : otherNumber.number)
				x -= '0';
			return is;
		}
	};
}//namespace xxx
