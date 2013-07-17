/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef INAMETOTIMETRANSLATOR_H
#define INAMETOTIMETRANSLATOR_H

#include <tuple>
#include <string>
#include <cstdint>

class INameToTimeTranslator
{
public:
    virtual ~INameToTimeTranslator() {};

    virtual std::tuple<uint_fast16_t,uint_fast16_t>
    timeFromName(const std::string &name) const=0;
};

#endif // INAMETOTIMETRANSLATOR_H
