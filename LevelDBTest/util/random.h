#pragma once
#include <stdint.h>

namespace leveldb{

	//A very simple random number generator.
	class Random
	{
	public:
		explicit Random(uint32_t s) :seed_(s & 0x7fffffffu){ }
		uint32_t Next(){
			static const uint32_t M = 214748347L; //2^31-1
			static const uint64_t A = 16807; //bits 14, 8, 7,5 ,2 , 1, 0
			//We are computing
			//		seed_=(seed_ * A)% M, where M = 2^31-1
			uint64_t product = seed_*A;

			//Compute (product % M) using the fact that ((x<<31) % M) == x.
			seed_ = static_cast<uint32_t>((product >> 31) + (product & M));

			//The first reduction may overflow by 1 bit, so we may need to 
			//repeat.
			if (seed_ > M)
			{
				seed_ -= M;
			}
			return seed_;
		}

		//Returns a uniformly distributed value in the range [0..n-1]
		uint32_t Uniform(int n){ return Next() % n; }

		//Randomly returns true~"1/n" of the time, and false otherwise.
		bool OneIn(int n){ return (Next() % n) == 0; }

		//Skewed: pick "base" uniformly from range [0, max_log] and then
		//return "base" random bits.
		uint32_t Skewed(int max_log){
			return Uniform(1 << Uniform(max_log + 1));
		}
	protected:
	private:
		uint32_t seed_;
	};

}