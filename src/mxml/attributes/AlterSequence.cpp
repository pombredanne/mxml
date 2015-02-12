//  Created by Alejandro Isaza on 2015-02-12.
//  Copyright (c) 2015 Venture Media Labs. All rights reserved.

#include "AlterSequence.h"

#include <mxml/dom/Measure.h>
#include <mxml/dom/Part.h>

#include <algorithm>
#include <iterator>


namespace mxml {

void AlterSequence::addFromNote(std::size_t partIndex, std::size_t measureIndex, const dom::Note& note) {
    if (!note.pitch())
        return;

    auto& pitch = *note.pitch();
    if (!pitch.alter().isPresent())
        return;

    Item item;
    item.index = indexFromNote(note);
    item.value = pitch.alter();
    _items.push_back(item);
}

int AlterSequence::find(const Index& index, int defaultAlter) const {
    Item modelItem;
    modelItem.index = index;

    // Only note alters on the same measure should be considered
    auto range = std::equal_range(_items.begin(), _items.end(), modelItem, [=](const Item& item1, const Item& item2) {
        return item1.index.time.measureIndex < item2.index.time.measureIndex;
    });

    // Find the last alter with a time before index
    auto rbegin = std::reverse_iterator<std::vector<Item>::const_iterator>(range.second);
    auto rend = std::reverse_iterator<std::vector<Item>::const_iterator>(range.first);
    auto it = std::find_if(rbegin, rend, [=](const Item& item) {
        return item.index.line == modelItem.index.line && item.index.time.time < modelItem.index.time.time;
    });
    if (it == rend)
        return defaultAlter;

    return it->value;
}

} // namespace mxml
