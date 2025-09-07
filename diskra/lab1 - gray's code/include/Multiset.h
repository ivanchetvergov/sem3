// Multiset.h
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

class Multiset {
public:
    Multiset() : totalCardinality_(0) {}

    long long getCardinality() const { return totalCardinality_; }
    const std::map<std::string, int>& getElements() const { return elements_; }

    Multiset operator+(const Multiset& other) const;
    Multiset operator-(const Multiset& other) const;
    Multiset operator*(const Multiset& other) const;
    Multiset operator/(const Multiset& other) const;
    Multiset operator|(const Multiset& other) const; 
    Multiset operator&(const Multiset& other) const; 
    Multiset operator^(const Multiset& other) const;

    void fillRandomly(int n);
    void fillManually(const Multiset& universe);
    void fillAutomatically(const Multiset& universe, int desiredCardinality);

    bool isEmpty() const;
    void print() const;

private:
    std::map<std::string, int> elements_;
    unsigned long long totalCardinality_;

    Multiset unionWith(const Multiset& other) const;
    Multiset intersectWith(const Multiset& other) const;
    Multiset difference(const Multiset& other) const;
    Multiset symmetricDifference(const Multiset& other) const;
    Multiset complement(const Multiset& universe) const;
};