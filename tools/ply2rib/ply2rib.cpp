#include "ply.hpp"

#include <boost/program_options.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace po = boost::program_options;
using namespace std;
using namespace std::tr1;
using namespace std::tr1::placeholders;

class Ply2Rib
{
public:
    Ply2Rib();
    
    bool convert(const string &plyName, istream& plyIStream, ostream& ribOStream);

// PLY handling
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
    void comment_callback(const string& filename, const string& content);

    // Vertex callbacks
    void vertex_begin();
    void vertex_end();
    void vertex_x_callback(ply::float32 x);
    void vertex_y_callback(ply::float32 y);
    void vertex_z_callback(ply::float32 z);
    void vertex_nx_callback(ply::float32 nx);
    void vertex_ny_callback(ply::float32 ny);
    void vertex_nz_callback(ply::float32 nz);

    // Faces callbacks
    void face_begin();
    void face_vertex_indices_begin(ply::uint8 size);
    void face_vertex_indices_element(ply::uint32 vertex_index);
    void face_vertex_indices_end();
    void face_end();

// Outputting to RIB
private:
    void writeRIB();

private:
    struct Vertex
    {
        Vertex() : x(0.f), y(0.f), z(0.f) {}
        float x, y, z;
    };
    typedef Vertex VertexNormal;

    friend ostream& operator<<(ostream &out, const Ply2Rib::Vertex &v);
    
    struct Face
    {
        Face() : verticesCount(0) {}
        size_t verticesCount;
        vector<size_t>  vertexIndices;
    };

private:
    ostream* rib;

    bool perVertexNormalsSpecified;

    Vertex currentVertex;
    vector<Vertex> vertices;

    VertexNormal currentNormal;
    vector<VertexNormal> verticesNormals;

    Face currentFace;
    vector<Face> faces;
};

Ply2Rib::Ply2Rib()
    :   rib(nullptr),
        perVertexNormalsSpecified(false)
{
}

//---------------------------------------------------------------------------------
// Internal types

ostream& operator<<(ostream &out, const Ply2Rib::Vertex &v)
{
    out << v.x << " " << v.y << " " << v.z;
    return out;
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
    else if (element_name == "face") {
        return tuple<function<void()>, function<void()> >(
                bind(&Ply2Rib::face_begin, this),
                bind(&Ply2Rib::face_end, this)
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
        if (property_name == "nx")
        {
            perVertexNormalsSpecified = true;
            return bind(&Ply2Rib::vertex_nx_callback, this, _1);
        }
        else if (property_name == "ny")
        {
            perVertexNormalsSpecified = true;
            return bind(&Ply2Rib::vertex_ny_callback, this, _1);
        }
        else if (property_name == "nz")
        {
            perVertexNormalsSpecified = true;
            return bind(&Ply2Rib::vertex_nz_callback, this, _1);
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
       function<void (ply::uint32)>,
       function<void ()>
     >
Ply2Rib::list_property_definition_callback(const string& element_name, const string& property_name)
{
    if (element_name == "face")
    {
        if (property_name == "vertex_index" || property_name == "vertex_indices")
        {
            return tuple<function<void (ply::uint8)>, function<void (ply::uint32)>, function<void ()> >(
                    bind(&Ply2Rib::face_vertex_indices_begin, this, _1),
                    bind(&Ply2Rib::face_vertex_indices_element, this, _1),
                    bind(&Ply2Rib::face_vertex_indices_end, this)
                );
        }
    }

    return tuple<function<void (ply::uint8)>, function<void (ply::int32)>, function<void ()> >(0, 0, 0);
}

template <>
tuple< function<void (ply::uint8)>,
       function<void (ply::int32)>,
       function<void ()>
     >
Ply2Rib::list_property_definition_callback(const string& element_name, const string& property_name)
{
    return list_property_definition_callback<ply::uint8, ply::uint32>(element_name, property_name);
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

void Ply2Rib::comment_callback(const string& filename, const string& content)
{
    // Strip out the "comment " prefix
    (*rib) << "# Comment from original ply (" << filename << "): " << content.substr(8) << endl;
}

//---------------------------------------------------------------------------------
// Vertices functions

void Ply2Rib::vertex_begin()
{
}

void Ply2Rib::vertex_end()
{
    vertices.push_back(currentVertex);

    if (perVertexNormalsSpecified)
        verticesNormals.push_back(currentNormal);
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

void Ply2Rib::vertex_nx_callback(ply::float32 nx)
{
    currentNormal.x = nx;
}

void Ply2Rib::vertex_ny_callback(ply::float32 ny)
{
    currentNormal.y = ny;
}

void Ply2Rib::vertex_nz_callback(ply::float32 nz)
{
    currentNormal.z = nz;
}

//---------------------------------------------------------------------------------
// Faces functions

void Ply2Rib::face_begin()
{
    currentFace.vertexIndices.clear();
}

void Ply2Rib::face_end()
{
    faces.push_back(currentFace);
}

void Ply2Rib::face_vertex_indices_begin(ply::uint8 size)
{
    currentFace.verticesCount = size;
}

void Ply2Rib::face_vertex_indices_element(ply::uint32 vertex_index)
{
    currentFace.vertexIndices.push_back(vertex_index);
}

void Ply2Rib::face_vertex_indices_end()
{
}

//---------------------------------------------------------------------------------
// Conversion

void Ply2Rib::writeRIB()
{
    ostream &out(*rib);

    out << endl << "AttributeBegin" << endl << "\t";
    out << "PointsPolygons ";

        // Write how many vertices per faces we have
        out << "[";
            for_each(faces.begin(), faces.end(), [&] (const Face &f) { out << f.verticesCount << " "; } );
        out << "]";

        // Write our vertices' indices for each face
        out << endl << "\t\t[";
        for_each(faces.begin(), faces.end(),
            [&] (const Face &f)
            {
                copy(f.vertexIndices.begin(),
                     f.vertexIndices.end(),
                     ostream_iterator<size_t>(out, " "));
            } );
        ostream::pos_type pos = out.tellp();
        out.seekp(pos - ostream::pos_type(1));
        out << "]";

        // Write our vertices' coordinates
        out << endl << "\t\t\"P\" [";
        copy(vertices.begin(), vertices.end(), ostream_iterator<Vertex>(out, "\n\t\t     "));
        pos = out.tellp();
        out.seekp(pos - ostream::pos_type(1));
        out << "]";

        // Write our optional per-vertex normals
        if (perVertexNormalsSpecified)
        {
            out << endl << "\t\t\"N\" [";
            copy(verticesNormals.begin(), verticesNormals.end(), ostream_iterator<VertexNormal>(out, "\n\t\t     "));
            pos = out.tellp();
            out.seekp(pos - ostream::pos_type(1));
            out << "]";
        }

    out << endl << "AttributeEnd" << endl;
}

bool Ply2Rib::convert(const string &plyName, istream& plyIStream, ostream& ribOStream)
{
    rib = &ribOStream;
    (*rib) << "##RenderMan RIB-Structure 1.1" << endl;
    (*rib) << "##Creator pprt::ply2rib (from " << plyName << ")" << endl << endl;

    ply::ply_parser ply_parser;

    ply_parser.info_callback(bind(&Ply2Rib::info_callback, this, ref(plyName), _1, _2));
    ply_parser.warning_callback(bind(&Ply2Rib::warning_callback, this, ref(plyName), _1, _2));
    ply_parser.error_callback(bind(&Ply2Rib::error_callback, this, ref(plyName), _1, _2)); 
    ply_parser.comment_callback(bind(&Ply2Rib::comment_callback, this, ref(plyName), _1)); 

    // Set our scalar callback (for vertices' coordinates etc...)
    ply::ply_parser::scalar_property_definition_callbacks_type scalar_property_definition_callbacks;
    ply::at<ply::float32>(scalar_property_definition_callbacks) = bind(&Ply2Rib::scalar_property_definition_callback<ply::float32>, this, _1, _2);
    ply_parser.scalar_property_definition_callbacks(scalar_property_definition_callbacks);

    // Set our element callback (for vertices' indices etc...)
    ply::ply_parser::list_property_definition_callbacks_type list_property_definition_callbacks;
    ply::at<ply::uint8, ply::uint32>(list_property_definition_callbacks) = bind(&Ply2Rib::list_property_definition_callback<ply::uint8, ply::uint32>, this, _1, _2);
    ply_parser.list_property_definition_callbacks(list_property_definition_callbacks);

    // Set our property callback (to handle begin/end of elements)
    ply_parser.element_definition_callback(bind(&Ply2Rib::element_definition_callback, this, _1, _2));

    bool res = ply_parser.parse(plyIStream);
    writeRIB();
    return res;
}

//---------------------------------------------------------------------------------
// Main + utility

string getDefaultRibFileFrom(const string &inFile)
{
    return string(inFile).replace(inFile.size() - 3, 3, "rib");
}

int main(int argc, char **argv)
{
    bool waitInputBeforeQuit = false;

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
    if ((vm.count("help") != 0 || vm.count("in") == 0) && argc != 2)
    {
        cout << desc << endl;
        return 0;
    }

    // The "downside" to not using argument
    if (argc == 2)
        waitInputBeforeQuit = true;

    // Get input file (try to interpret the 2nd argument as a ply filepath, if user drag&dropped a file)
    const string plyFile = vm.count("in") != 0 ? vm["in"].as<string>() : argv[1];
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

    if (waitInputBeforeQuit)
    {
        cout << "Press any key to leave...";
        cin.get();
    }

    return 0;
}
