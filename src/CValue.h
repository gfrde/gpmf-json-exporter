//
// Created by georg on 02.04.25.
//

#ifndef CVALUE_H
#define CVALUE_H
#include <list>
#include <string>


class CValue {
public:
    virtual std::string getAsJsonValue() const;
};

class CStringValue : public CValue {
public:
    explicit CStringValue(std::string v);
    std::string getAsJsonValue() const override;

public:
    std::string value;
};

class CLongValue : public CValue {
public:
    explicit CLongValue(long v);
    std::string getAsJsonValue() const override;

public:
    long value;
};

class CLongLongValue : public CValue {
public:
    explicit CLongLongValue(long long v);
    std::string getAsJsonValue() const override;

public:
    long long value;
};

class CDoubleValue : public CValue {
public:
    explicit CDoubleValue(double v);
    std::string getAsJsonValue() const override;

public:
    double value;
};

class CListValue : public CValue {
public:
    explicit CListValue();
    std::string getAsJsonValue() const override;
    void addValue(CValue v);

public:
    std::list<CValue> value;
};


#endif //CVALUE_H
