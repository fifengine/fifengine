// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_TEXT_H
#define FIFE_EVENTCHANNEL_TEXT_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

    /** Represents a text.
     */
    class Text
    {
    public:
        /** Constructor
         * @param text pointer to the UTF-8 text
         * @param start point of the text edit event
         * @param length length of the text edit event
         */
        explicit Text(const char* text = nullptr, int32_t start = 0, int32_t length = 0) :
            m_text(text ? text : ""), m_start(start), m_length(length)
        {
        }

        Text(const Text&)            = default;
        Text& operator=(const Text&) = default;

        /** Destructor
         */
        virtual ~Text() { }

        /** Tells whether the Text can be represented as a string.
         */
        bool isPrintable() const
        {
            if (!m_text.empty()) {
                return true;
            }
            return false;
        }

        /** Gets the string, encoded as UTF-8.
         */
        std::string getAsString() const
        {
            return m_text;
        }

        /** Gets the start point of the text edit event, cursor position.
         */
        int32_t getStart() const
        {
            return m_start;
        }

        /** Gets the length of the text edit event,
         * the number of characters to edit from the start point.
         */
        int32_t getLength() const
        {
            return m_length;
        }

    private:
        std::string m_text;
        int32_t m_start;
        int32_t m_length;
    };

} // namespace FIFE

#endif
