#pragma once

#include "ini.h"

#include <string>
#include <vector>

namespace inifile {
template <typename CharType>
struct StringsIOHandler : public inifile::IOHandler<CharType>
{
    using Base = inifile::IOHandler<CharType>;
    explicit StringsIOHandler(int64_t alloc_size = 128, FlowDirection direction = FlowDirection::In)
        : _direction(direction)
    {
        _data.reserve(alloc_size);
        _current = _data.begin();
    }
    virtual ~StringsIOHandler() = default;
    bool read_line(typename Base::StrType& line) override
    {
        if (_direction != FlowDirection::In || eof())
            return false;
        line = *_current++;
        return true;
    }

    bool write_line(typename Base::StrViewType line) override
    {
        if (_direction != FlowDirection::Out)
            return false;
        if (eof())
        {
            // Might realloc, so can not increment pointer
            _data.emplace_back(line.data(), line.length());
            _current = _data.end();
        }
        else
            *_current++ = line;
        return true;
    }

    bool open_device(FlowDirection open_mode = FlowDirection::In) override
    {
        if (_direction != open_mode)
        {
            _direction = open_mode;
            _current = _data.begin();
        }
        return true;
    }

    bool close_device() override
    {
        return true;
    }

    bool is_opened(FlowDirection open_mode) override
    {
        return _direction == open_mode;
    }

    void reopen(FlowDirection direction)
    {
        // Consistent with FILE interface
        if (direction == FlowDirection::Out)
            _data.clear();
        _direction = direction;
        _current = _data.begin();
    }

    bool eof() const
    {
        return _current == _data.end();
    }
    using Container = std::vector<typename Base::StrType>;
    Container _data;
    typename Container::iterator _current;
    FlowDirection _direction = FlowDirection::In;
};
} // namespace inifile