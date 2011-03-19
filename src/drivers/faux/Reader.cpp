/******************************************************************************
* Copyright (c) 2011, Michael P. Gerlek (mpg@flaxen.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <cassert>
#include <iostream>

#include <libpc/drivers/faux/Reader.hpp>
#include <libpc/drivers/faux/Iterator.hpp>
#include <libpc/Utils.hpp>
#include <libpc/exceptions.hpp>
#include <libpc/Iterator.hpp>

using std::vector;
using std::string;
using std::cout;

namespace libpc { namespace drivers { namespace faux {

Reader::Reader(const Bounds<double>& bounds, int numPoints, Mode mode)
    : libpc::Stage()
    , m_mode(mode)
{
    Header* header = new Header;
    Schema& schema = header->getSchema();

    schema.addDimension(Dimension(Dimension::Field_X, Dimension::Double));
    schema.addDimension(Dimension(Dimension::Field_Y, Dimension::Double));
    schema.addDimension(Dimension(Dimension::Field_Z, Dimension::Double));
    schema.addDimension(Dimension(Dimension::Field_Time, Dimension::Uint64));

    header->setNumPoints(numPoints);
    header->setBounds(bounds);

    setHeader(header);

    return;
}

Reader::Reader(const Bounds<double>& bounds, int numPoints, Mode mode, const std::vector<Dimension>& dimensions)
    : libpc::Stage()
    , m_mode(mode)
{
    Header* header = new Header;

    Schema& schema = header->getSchema();
    if (dimensions.size() == 0)
    {
        throw; // BUG
    }
    schema.addDimensions(dimensions);

    header->setNumPoints(numPoints);
    header->setBounds(bounds);

    setHeader(header);

    return;
}


const std::string& Reader::getName() const
{
    static std::string name("Faux Reader");
    return name;
}


Reader::Mode Reader::getMode() const
{
    return m_mode;
}


libpc::Iterator* Reader::createIterator() const
{
    return new Iterator(*this);
}


} } } // namespaces
