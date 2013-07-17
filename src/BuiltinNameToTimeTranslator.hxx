/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef BUILTINNAMETOTIMETRANSLATOR_H
#define BUILTINNAMETOTIMETRANSLATOR_H

#include "INameToTimeTranslator.hxx"

class BuiltinNameToTimeTranslator : public INameToTimeTranslator
{
public:
    virtual std::tuple< uint_fast16_t, uint_fast16_t >
    timeFromName(const std::string& name) const override;
    
};

#endif // BUILTINNAMETOTIMETRANSLATOR_H
