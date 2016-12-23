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

#include "PipelineKernel.hpp"

#ifdef PDAL_HAVE_LIBXML2
#include <pdal/XMLSchema.hpp>
#endif

#include <pdal/PDALUtils.hpp>
#include <pdal/pdal_macros.hpp>

namespace pdal
{

static PluginInfo const s_info = PluginInfo("kernels.pipeline",
    "Pipeline Kernel", "http://pdal.io/apps/pipeline.html" );

CREATE_STATIC_PLUGIN(1, 0, PipelineKernel, Kernel, s_info)

std::string PipelineKernel::getName() const { return s_info.name; }

PipelineKernel::PipelineKernel() : m_validate(false), m_progressFd(-1)
{}


void PipelineKernel::validateSwitches(ProgramArgs& args)
{
    if (m_usestdin)
        m_inputFile = "STDIN";

    if (m_inputFile.empty())
        throw pdal_error("Input filename required.");
}


void PipelineKernel::addSwitches(ProgramArgs& args)
{
    args.add("input,i", "input file name", m_inputFile).setOptionalPositional();
    args.add("pipeline-serialization", "Output file for pipeline serialization",
        m_pipelineFile);
    args.add("validate", "Validate the pipeline (including serialization), "
        "but do not write points", m_validate);
    args.add("progress",
        "Name of file or FIFO to which stages should write progress "
        "information.  The file/FIFO must exist.  PDAL will not create "
        "the progress file.",
        m_progressFile);
    args.add("pointcloudschema", "dump PointCloudSchema XML output",
        m_PointCloudSchemaOutput).setHidden();
    args.add("stdin,s", "Read pipeline from standard input", m_usestdin);
    args.add("stream", "Attempt to run pipeline in streaming mode.", m_stream);
}

int PipelineKernel::execute()
{
    if (!Utils::fileExists(m_inputFile))
        throw pdal_error("file not found: " + m_inputFile);
    if (m_progressFile.size())
        m_progressFd = Utils::openProgress(m_progressFile);

    m_manager.readPipeline(m_inputFile);

    if (m_validate)
    {
        // Validate the options of the pipeline we were
        // given, and once we succeed, we're done
        m_manager.prepare();
        Utils::closeProgress(m_progressFd);
        return 0;
    }

    if (m_stream)
    {
        FixedPointTable table(10000);
        m_manager.executeStream(table);
    }
    else
        m_manager.execute();

    if (m_pipelineFile.size() > 0)
        PipelineWriter::writePipeline(m_manager.getStage(), m_pipelineFile);

    if (m_PointCloudSchemaOutput.size() > 0)
    {
#ifdef PDAL_HAVE_LIBXML2
        XMLSchema schema(m_manager.pointTable().layout());

        std::ostream *out = Utils::createFile(m_PointCloudSchemaOutput);
        std::string xml(schema.xml());
        out->write(xml.c_str(), xml.size());
        Utils::closeFile(out);
#else
        std::cerr << "libxml2 support not available, no schema is produced" <<
            std::endl;
#endif

    }
    Utils::closeProgress(m_progressFd);
    return 0;
}

} // pdal
