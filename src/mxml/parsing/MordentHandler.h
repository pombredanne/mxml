//  Created by Alejandro Isaza on 2014-06-23.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#pragma once
#include <lxml/BaseRecursiveHandler.h>
#include <mxml/dom/Mordent.h>

namespace mxml {

class MordentHandler : public lxml::BaseRecursiveHandler<std::unique_ptr<dom::Mordent>> {
public:
    void startElement(const lxml::QName& qname, const AttributeMap& attributes);
};

} // namespace
