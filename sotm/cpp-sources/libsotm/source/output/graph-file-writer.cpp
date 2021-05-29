#include "sotm/output/graph-file-writer.hpp"

#include <vtkCellData.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>
#include <sstream>

using namespace sotm;

FileWriter::FileWriter(ModelContext* modelContext, RenderPreferences* renderPreferences) :
		m_modelContext(modelContext),
		m_renderPreferences(renderPreferences)
{
	clear();
}

void FileWriter::write(const std::string& filename)
{
	clear();

	writeLinks(filename + ".vtp");
	writeNodes(filename + ".csv");
}

void FileWriter::writeLinks(const std::string& filename)
{
	buildLinks();
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetInputData(polyData);
	writer->SetFileName(filename.c_str());
	writer->Write();
}

void FileWriter::writeNodes(const std::string& filename)
{
	m_oss << "x, y, z, parameter1, parameter2, parameter3" << std::endl;

	std::ofstream output(filename.c_str(), std::ios::out);
	if (!output.is_open())
	{
		std::cerr << "Cannot open file " << filename << std::endl;
		return;
	}

	m_modelContext->graphRegister.applyNodeVisitor(
		[this] (sotm::Node* node)
		{
			nodeVisitor(node);
		}
	);

	output << m_oss.str();
}

void FileWriter::buildLinks()
{
	m_modelContext->graphRegister.applyLinkVisitor(
		[this] (sotm::Link* link)
		{
			linkVisitor(link);
		}
	);
	polyData->SetPoints(points);
	polyData->SetLines(linesCellArray);
	polyData->GetCellData()->SetScalars(linesDoubleArray);
}

void FileWriter::linkVisitor(sotm::Link* link)
{
	auto& p1 = link->getNode1()->pos;
	auto& p2 = link->getNode2()->pos;
	//Vector<3> center = (p1 + p2) / 2.0;

	vtkIdType id1 = points->InsertNextPoint(p1.x);
	vtkIdType id2 = points->InsertNextPoint(p2.x);

	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, id1);
	line->GetPointIds()->SetId(1, id2);

	double parameters[3] = {0.0, 0.0, 0.0};
	link->payload->getParametersVector(parameters);
	linesDoubleArray->InsertNextTypedTuple(parameters);
	//buffer->colors->InsertNe
	linesCellArray->InsertNextCell(line);
}

void FileWriter::nodeVisitor(sotm::Node* node)
{
	double parameters[3] = {0.0, 0.0, 0.0};
	node->payload->getParametersVector(parameters);
	m_oss << node->pos.x[0] << ", "
		<< node->pos.x[1] << ", "
		<< node->pos.x[2] << ", "
		<< parameters[0] << ", "
		<< parameters[1] << ", "
		<< parameters[2] << std::endl;
}

void FileWriter::clear()
{
	points->Reset();
	linesCellArray->Reset();
	polyData->Reset();
	linesDoubleArray->Reset();
	linesDoubleArray->SetNumberOfComponents(3);
	m_oss.clear();
	m_oss.str("");
}

//////////////////////////////////////////////////////////
// FileWriteHook
FileWriteHook::FileWriteHook(ModelContext* modelContext, RenderPreferences* renderPreferences, double period) :
		m_writer(modelContext, renderPreferences)
{
	setPeriod(period);
}

void FileWriteHook::setFilenamePrefix(const std::string& prefix)
{
	m_prefix = prefix;
}

void FileWriteHook::hook(double time, double wantedTime)
{
	UNUSED_ARG(wantedTime);
	std::ostringstream ss;
	ss << m_prefix << "_t=" << std::fixed << std::setprecision(10) << time;
	m_writer.write(ss.str().c_str());
}
