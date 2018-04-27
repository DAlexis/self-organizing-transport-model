#include "links-finder.hpp"

#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkActor.h>

using namespace std;

LinksFinder::LinksFinder(const std::string& filename) :
    m_filname(filename)
{

}

bool LinksFinder::getMinMaxZ(double tempThreshold, double& min, double& max)
{

    vtkSmartPointer<vtkXMLPolyDataReader> reader =
      vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(m_filname.c_str());
    reader->Update();

    vtkPolyData *linesPolyData = reader->GetOutput();

    linesPolyData->GetLines()->InitTraversal();
    vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
    vtkIdType cellid = 0;

    bool initialized = false;

    while(linesPolyData->GetLines()->GetNextCell(idList))
    {
        // Getting tuple associated with cell (cell is line)
        double *tuple = linesPolyData->GetCellData()->GetScalars()->GetTuple3(cellid++);

        if (tuple[1] >= tempThreshold)
        {
            for(vtkIdType pointId = 0; pointId < idList->GetNumberOfIds(); pointId++)
            {
                // Getting point global id
                vtkIdType p = idList->GetId(pointId);
                double point[3];

                // Getting point coordinates by global id
                linesPolyData->GetPoint(p, point);

                double z = point[2];

                if (!initialized)
                {
                    max = min = z;
                    initialized = true;
                }

                if (z > max)
                {
                    max = z;
                }

                if (z < min)
                {
                    min = z;
                }
            }
        }
    }
    return initialized;
}
