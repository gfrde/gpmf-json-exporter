//
// Created by georg on 02.04.25.
//

#ifndef CVALUE_H
#define CVALUE_H
#include <list>
#include <string>


class CValue {
public:
    std::string getAsJsonValue() const;
};

class CStringValue : public CValue {
public:
    explicit CStringValue(std::string v);
    std::string getAsJsonValue() const;

public:
    std::string value;
};

class CLongValue : public CValue {
public:
    explicit CLongValue(long v);
    std::string getAsJsonValue() const;

public:
    long value;
};

class CDoubleValue : public CValue {
public:
    explicit CDoubleValue(double v);
    std::string getAsJsonValue() const;

public:
    double value;
};

class CListValue : public CValue {
public:
    explicit CListValue();
    std::string getAsJsonValue() const;
    void addValue(CValue v);

public:
    std::list<CValue> value;
};


#endif //CVALUE_H
