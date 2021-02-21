#include "utils.hpp"

std::string generateName()
{
  // Const list of vowels and consonants, in decreasing letter use frequency in the English language
  const char vowels[5] = {'e','a','o','i','u'};
  const char consonants[21] = {'t','n','s','h','r','d','l','c','m','f','w','y','p','v','b','g','k','q','j','x','z'};

  // String stream and string variables
  std::ostringstream nameStream;
  std::string nameString;

  // Anywhere between 2-4 “syllables”
  for (int i = 0; i < (rand() % 3) + 2; i++)
  {
    // Iterate through consonants
    for (int j = 0; j < sizeof(consonants); j++)
      // If decreasing frequency occurs on letter, add the letter and break out
      if (rand() % ((j*2) + 3) == 0) { nameStream << consonants[j]; break; }

    // Iterate through vowels
    for (int j = 0; j < sizeof(vowels); j++)
      // If decreasing frequency occurs on letter, add the letter and break out
      if (rand() % ((j*2) + 3) == 0) { nameStream << vowels[j]; break; }

    // Iterate through vowels again, 50% chance
    if (rand() % 2 == 0)
      for (int j = 0; j < sizeof(vowels); j++)
        // If decreasing frequency occurs on letter, add the letter and break out
        if (rand() % ((j*2) + 3) == 0) { nameStream << vowels[j]; break; }

    // Iterate through consonants again, 33% chance
    if (rand() % 3 == 0)
      for (int j = 0; j < sizeof(consonants); j++)
        // If decreasing frequency occurs on letter, add the letter and break out
        if (rand() % ((j*2) + 3) == 0) { nameStream << consonants[j]; break; }
  }
  // Convert stream to string
  nameString = nameStream.str();
  // Make first letter uppercase
  nameString[0] = toupper(nameString[0]);
  // Return this string
  return nameString;
}