//  Created by Alejandro Isaza on 2014-07-09.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#pragma once
#include "Node.h"

namespace mxml {
namespace dom {

class Syllabic : public Node {
public:
    enum Type {
        kSingle,
        kBegin,
        kEnd,
        kMiddle
    };

public:
    Syllabic() : _type(kSingle) {}

    Type type() const {
        return _type;
    }
    void setType(Type type) {
        _type = type;
    }
    
private:
    Type _type;
};

} // namespace dom
} // namespace mxml
