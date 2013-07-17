/*
 * Copyright 2013  Maciej Poleski
 */

#include "BuiltinNameToTimeTranslator.hxx"

#include <stdexcept>

std::tuple< uint_fast16_t, uint_fast16_t >
BuiltinNameToTimeTranslator::timeFromName(const std::string& name) const
{
    if(name=="wieczór")
    {
        return std::make_tuple<uint_fast16_t, uint_fast16_t>(22,00);
    }
    else if(name=="rano")
    {
        return std::make_tuple<uint_fast16_t, uint_fast16_t>(12,00);
    }
    else if(name=="popołudnie")
    {
        return std::make_tuple<uint_fast16_t, uint_fast16_t>(16,00);
    }
    else
    {
        throw std::runtime_error("Unknown time name "+name);
    }
}
