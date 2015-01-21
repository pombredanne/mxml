//  Created by Alejandro Isaza on 2014-03-18.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#pragma once
#include "Node.h"
#include "Optional.h"
#include "Position.h"

#include <string>

namespace mxml {
namespace dom {

class DirectionType : public Node {
public:
    DirectionType() = default;
    virtual ~DirectionType() = default;
    
    /** Return true if this direction type has start and stop elements. */
    virtual bool span() const = 0;
    
public:
    Position position;
};

class Dynamics : public DirectionType {
public:
    const std::string& string() const {
        return _string;
    }
    void setString(const std::string& string) {
        _string = string;
    }
    
    bool span() const {
        return false;
    }
    
private:
    std::string _string;
};

class Words : public DirectionType {
public:
    const std::string& contents() const {
        return _contents;
    }
    void setContents(const std::string& contents) {
        _contents = contents;
    }
    
    bool span() const {
        return false;
    }
    
private:
    std::string _contents;
};

class Segno : public DirectionType {
public:
    bool span() const {
        return false;
    }
};

class Coda : public DirectionType {
public:
    bool span() const {
        return false;
    }
};

} // namespace dom
} // namespace mxml
