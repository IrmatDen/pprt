#include "ply.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <vector>

namespace po = boost::program_options;
using namespace std;
using namespace placeholders;

class Ply2Rib
{
public:
    // Constructing the object start the actual conversion
    Ply2Rib();
    
    bool convert(const string &plyName, istream& plyIStream, ostream& ribOStream);

private:
    struct Vertex
    {
        float x, y, z;
    };
    
    typedef vector<size_t>  Face;

private:
    // Setup
    tuple<function<void()>, function<void()> > element_definition_callback(const string& element_name, size_t count);

    template <typename ScalarType>
    function<void (ScalarType)> scalar_property_definition_callback(const string& element_name, const string& property_name);
    
    template <typename SizeType, typename ScalarType>
    tuple<function<void (SizeType)>, function<void (ScalarType)>, function<void ()> >
        list_property_definition_callback(const string& element_name, const string& property_name);

    // Misc callback
    void info_callback(const string& filename, size_t line_number, const string& message);
    void warning_callback(const string& filename, size_t line_number, const string& message);
    void error_callback(const string& filename, size_t line_number, const string& message);

    // Vertex callbacks
    void vertex_begin();
    void vertex_x_callback(ply::float32 x);
    void vertex_y_callback(ply::float32 y);
    void vertex_z_callback(ply::float32 z);
    void vertex_end();

    // Faces callbacks
    void face_vertex_indices_begin(ply::uint8 size);
    void face_vertex_indices_element(ply::int32 vertex_index);
    void face_vertex_indices_end();

private:
    ostream* rib;

    Vertex currentVertex;
    vector<Vertex> vertices;

    Face currentFace;
    vector<Face> faces;
};

Ply2Rib::Ply2Rib()
    : rib(nullptr)
{
}

//---------------------------------------------------------------------------------
// Setup functions

tuple<function<void()>, function<void()> > Ply2Rib::element_definition_callback(const string& element_name, size_t count)
{
    if (element_name == "vertex")
    {
        return tuple<function<void()>, function<void()> >(
                bind(&Ply2Rib::vertex_begin, this),
                bind(&Ply2Rib::vertex_end, this)
            );
    }
    else
    {
        return tuple<function<void()>, function<void()> >(0, 0);
    }
}

template <>
function<void (ply::float32)> Ply2Rib::scalar_property_definition_callback(const string& element_name, const string& property_name)
{
    if (element_name == "vertex")
    {
        if (property_name == "x")
        {
            return bind(&Ply2Rib::vertex_x_callback, this, _1);
        }
        else if (property_name == "y")
        {
            return bind(&Ply2Rib::vertex_y_callback, this, _1);
        }
        else if (property_name == "z")
        {
            return bind(&Ply2Rib::vertex_z_callback, this, _1);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

template <>
tuple< function<void (ply::uint8)>,
       function<void (ply::int32)>,
       function<void ()>
     >
Ply2Rib::list_property_definition_callback(const string& element_name, const string& property_name)
{
    if ((element_name == "face") && (property_name == "vertex_index"))
    {
        return tuple<function<void (ply::uint8)>, function<void (ply::int32)>, function<void ()> >(
                bind(&Ply2Rib::face_vertex_indices_begin, this, _1),
                bind(&Ply2Rib::face_vertex_indices_element, this, _1),
                bind(&Ply2Rib::face_vertex_indices_end, this)
            );
    }
    else
    {
        return tuple<function<void (ply::uint8)>, function<void (ply::int32)>, function<void ()> >(0, 0, 0);
    }
}

//---------------------------------------------------------------------------------
// Misc functions

void Ply2Rib::info_callback(const string& filename, size_t line_number, const string& message)
{
    cout << "[INFO] " << filename << " (line " << line_number << "): " << message << endl;
}

void Ply2Rib::warning_callback(const string& filename, size_t line_number, const string& message)
{
    cout << "[WARNING] " << filename << " (line " << line_number << "): " << message << endl;
}

void Ply2Rib::error_callback(const string& filename, size_t line_number, const string& message)
{
    cout << "[ERROR] " << filename << " (line " << line_number << "): " << message << endl;
}

//---------------------------------------------------------------------------------
// Vertices functions

void Ply2Rib::vertex_begin()
{
}

void Ply2Rib::vertex_end()
{
    vertices.push_back(currentVertex);
}

void Ply2Rib::vertex_x_callback(ply::float32 x)
{
    currentVertex.x = x;
}

void Ply2Rib::vertex_y_callback(ply::float32 y)
{
    currentVertex.y = y;
}

void Ply2Rib::vertex_z_callback(ply::float32 z)
{
    currentVertex.z = z;
}

//---------------------------------------------------------------------------------
// Faces functions

void Ply2Rib::face_vertex_indices_begin(ply::uint8 size)
{
    currentFace.reserve(size);
}

void Ply2Rib::face_vertex_indices_element(ply::int32 vertex_index)
{
    currentFace.push_back(vertex_index);
}

void Ply2Rib::face_vertex_indices_end()
{
    faces.push_back(currentFace);
    currentFace.clear();
}

//---------------------------------------------------------------------------------
// Conversion

bool Ply2Rib::convert(const string &plyName, istream& plyIStream, ostream& ribOStream)
{
    rib = &ribOStream;
    (*rib) << "##RenderMan RIB-Structure 1.1" << endl;
    (*rib) << "##Creator pprt::Ply2Rib (from " << plyName << ")" << endl;

    ply::ply_parser ply_parser;

    ply_parser.info_callback(bind(&Ply2Rib::info_callback, this, ref(plyName), _1, _2));
    ply_parser.warning_callback(bind(&Ply2Rib::warning_callback, this, ref(plyName), _1, _2));
    ply_parser.error_callback(bind(&Ply2Rib::error_callback, this, ref(plyName), _1, _2)); 

    // Set our scalar callback (for vertices' coordinates etc...)
    ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
    ply::at<ply::float32>(scalar_property_definition_callbacks) = bind(&Ply2Rib::scalar_property_definition_callback<ply::float32>, this, _1, _2);
    ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

    // Set our element callback (for vertices' indices etc...)
    ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
    ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = bind(&Ply2Rib::list_property_definition_callback<ply::uint8, ply::int32>, this, _1, _2);
    ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);

    // Set our property callback (to handle begin/end of elements)
    /*ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
    ply::at<ply::uint8, ply::int32>(list_property_definition_callbacks) = bind(&Ply2Rib::list_property_definition_callback<ply::uint8, ply::int32>, this, _1, _2);
    ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);*/

    return ply_parser.parse(plyIStream);
}

//---------------------------------------------------------------------------------
// Main + utility

string getDefaultRibFileFrom(const string &inFile)
{
    return string(inFile).replace(inFile.size() - 3, 3, "rib");
}

int main(int argc, char **argv)
{
    po::options_description desc("Usage");
    desc.add_options()
        ("help", "Display this message")
        ("in", po::value<string>(), "PLY file to convert. It must have a .ply extension")
        ("out", po::value<string>(), "RIB archive filepath to create from the conversion."
                                          "If not provided, will default to the same path and name than the file specified as <in>")
    ;

    // Validate input/display help
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help") != 0 || vm.count("in") == 0)
    {
        cout << desc << endl;
        return 0;
    }

    // Get input file
    const string plyFile = vm["in"].as<string>();
    if (plyFile.substr(plyFile.size() - 4) != ".ply")
    {
        cout << desc << endl;
        return 0;
    }

    // Get (or default-set) output file
    const string ribFile = vm.count("out") != 0 ? vm["out"].as<string>() : getDefaultRibFileFrom(plyFile);

    // Aaaaaannnndd.... let's convert
    cout << "Parsing " << plyFile << endl;
    cout << "Output set to: " << ribFile << endl << endl;

    ifstream in(plyFile);
    ofstream out(ribFile);
    if (!Ply2Rib().convert(ribFile, in, out))
        cout << "An error occured while parsing " << plyFile << endl;
    else
        cout << "Succesfully converted " << plyFile << " to " << ribFile << endl;

    return 0;
}
