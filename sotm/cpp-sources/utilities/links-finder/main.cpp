#include "links-finder.hpp"
#include "cic.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    cic::Parameters pars(
        "Links conditional finder",
        cic::ParametersGroup(
            "Main",
            "Main options",
            cic::Parameter<std::string>("input", "Name of .vtp file"),
            cic::Parameter<double>("threshold", "Minimal leader temperature", 3000),
            cic::Parameter<bool>("verbose", "Print 'min' and 'max' words")
        )
    );
    cic::PreconfiguredOperations::addGeneralOptions(pars);
    try {
        bool needRun = cic::PreconfiguredOperations::quickReadConfiguration(
            pars,
            {
                "links-finder.conf"
            },
            argc,
            argv
        );
        if (!needRun)
            return 0;
    } catch (std::exception &ex) {
        cerr << "Error while reading configuration: " << ex.what() << endl;
        return -1;
    }

    if (!pars["Main"].initialized("input"))
    {
        cerr << "Input file not set, use --input=<file>" << endl;
        return -1;
    }
    LinksFinder lf(pars["Main"].get<std::string>("input"));
    double zmin=0.0, zmax=0.0;
    if (lf.getMinMaxZ(pars["Main"].get<double>("threshold"), zmin, zmax))
    {
        if (pars["Main"].get<bool>("verbose"))
        {
            cout << "min: " << zmin << endl;
            cout << "max: " << zmax << endl;
        } else {
            cout << zmin << "," << zmax << endl;
        }
    } else {
        if (pars["Main"].get<bool>("verbose"))
        {
            cout << "no" << endl;
        } else {
            cout << "0.0,0.0" << endl;
        }
    }
    return 0;
}
