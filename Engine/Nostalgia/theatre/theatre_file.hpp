#ifndef THEATRE_FILE_H
#define THEATRE_FILE_H

namespace TheatreFile
{
    /// Forward Declaration
    struct TheatreData;

    Error ParseTheatreFile(Sarg inPathToFile, Shared<TheatreData>& outData);
    Error ParseTheatreFile(Farg<FileData> inFileData, Shared<TheatreData>& outData);
}

#endif // THEATRE_FILE_H
