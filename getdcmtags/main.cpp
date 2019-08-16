#include <stdio.h>
#include <stdlib.h>

#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dctypes.h"

#define VERSION "0.1g"

static OFString tagSpecificCharacterSet="";
static OFString tagPatientName="";
static OFString tagSOPInstanceUID="";
static OFString tagSeriesInstanceUID="";
static OFString tagStudyInstanceUID="";
static OFString tagModality="";
static OFString tagBodyPartExamined="";
static OFString tagProtocolName="";
static OFString tagRetrieveAETitle="";
static OFString tagStationAETitle="";
static OFString tagManufacturer="";
static OFString tagManufacturerModelName="";
static OFString tagStudyDescription="";
static OFString tagCodeValue="";
static OFString tagCodeMeaning="";
static OFString tagSeriesDescription="";
static OFString tagPatientID="";
static OFString tagPatientBirthDate="";
static OFString tagPatientSex="";
static OFString tagAccessionNumber="";
static OFString tagReferringPhysicianName="";
static OFString tagStudyID="";
static OFString tagSeriesNumber="";
static OFString tagSeriesDate="";
static OFString tagSeriesTime="";
static OFString tagAcquisitionDate="";
static OFString tagAcquisitionTime="";
static OFString tagSequenceName="";
static OFString tagScanningSequence="";
static OFString tagSequenceVariant="";
static OFString tagMagneticFieldStrength="";
static OFString tagStationName="";
static OFString tagDeviceSerialNumber="";
static OFString tagDeviceUID="";
static OFString tagSoftwareVersions="";
static OFString tagContrastBolusAgent="";
static OFString tagImageComments="";
static OFString tagSliceThickness="";

static std::string bookkeeperAddress="";


void sendBookkeeperPost(OFString filename, OFString fileUID, OFString seriesUID)
{
    if (bookkeeperAddress.empty())
    {
        return;
    }

    std::string cmd="wget -q --post-data=\"filename=";
    cmd.append(filename.c_str());
    cmd.append("&file_uid=");
    cmd.append(fileUID.c_str());
    cmd.append("&series_uid=");
    cmd.append(seriesUID.c_str());
    cmd.append("\" http://");
    cmd.append(bookkeeperAddress);
    cmd.append("/register-dicom -O /dev/null");

    system(cmd.data());
}


void writeErrorInformation(OFString dcmFile, OFString errorString)
{
    OFString filename=dcmFile+".error";
    errorString="ERROR: "+errorString;
    FILE* fp = fopen(filename.c_str(), "w+");

    if (fp==nullptr)
    {
        std::cout << "ERROR: Unable to write error file " << filename << std::endl;
    }

    fprintf(fp, "%s\n", errorString.c_str());
    fclose(fp);
    std::cout << errorString << std::endl;
}


static DcmSpecificCharacterSet charsetConverter;
static bool isConversionNeeded=false;

#define INSERTTAG(A,B,C)    conversionBuffer=""; \
                            if (isConversionNeeded) { \
                                if (!charsetConverter.convertString(B, conversionBuffer).good()) { \
                                    std::cout << "ERROR: Unable to convert charset for tag " << A << std::endl; \
                                    std::cout << "ERROR: Unable to process file " << dcmFile << std::endl; \
                                } \
                            } else { \
                                conversionBuffer=B; \
                            } \
                            fprintf(fp, "\"%s\": \"%s\",\n",A,conversionBuffer.c_str())

bool writeTagsFile(OFString dcmFile, OFString originalFile)
{
    OFString filename=dcmFile+".tags";
    FILE* fp = fopen(filename.c_str(), "w+");

    if (fp==nullptr)
    {
        std::cout << "ERROR: Unable to write tag file " << filename << std::endl;
        return false;
    }

    fprintf(fp, "{\n");
    OFString conversionBuffer="";

    INSERTTAG("SpecificCharacterSet",          tagSpecificCharacterSet,          "ISO_IR 100");
    INSERTTAG("Modality",                      tagModality,                      "MR");
    INSERTTAG("BodyPartExamined",              tagBodyPartExamined,              "BRAIN");
    INSERTTAG("ProtocolName",                  tagProtocolName,                  "COR T1 PIT(POST)");
    INSERTTAG("RetrieveAETitle",               tagRetrieveAETitle,               "STORESCP");
    INSERTTAG("StationAETitle",                tagStationAETitle,                "ANY-SCP");
    INSERTTAG("Manufacturer",                  tagManufacturer,                  "HERMES");
    INSERTTAG("ManufacturerModelName",         tagManufacturerModelName,         "Router");
    INSERTTAG("StudyDescription",              tagStudyDescription,              "NEURO^HEAD");
    INSERTTAG("CodeValue",                     tagCodeValue,                     "IMG11291");
    INSERTTAG("CodeMeaning",                   tagCodeMeaning,                   "MRI BRAIN PITUITARY WITH AND WITHOUT IV CONTRAST");
    INSERTTAG("SeriesDescription",             tagSeriesDescription,             "COR T1 POST");
    INSERTTAG("PatientName",                   tagPatientName,                   "Knight^Michael");
    INSERTTAG("PatientID",                     tagPatientID,                     "987654321");
    INSERTTAG("PatientBirthDate",              tagPatientBirthDate,              "20100101");
    INSERTTAG("PatientSex",                    tagPatientSex,                    "M");
    INSERTTAG("AccessionNumber",               tagAccessionNumber,               "1234567");
    INSERTTAG("ReferringPhysicianName",        tagReferringPhysicianName,        "Tanner^Willie");
    INSERTTAG("StudyID",                       tagStudyID,                       "243211348");
    INSERTTAG("SeriesNumber",                  tagSeriesNumber,                  "99");
    INSERTTAG("SOPInstanceUID",                tagSOPInstanceUID,                "1.2.256.0.7220020.3.1.3.541411159.31.1254476944.91518");
    INSERTTAG("SeriesInstanceUID",             tagSeriesInstanceUID,             "1.2.256.0.7230020.3.1.3.531431169.31.1254476944.91508");
    INSERTTAG("StudyInstanceUID",              tagStudyInstanceUID,              "1.2.226.0.7231010.3.1.2.531431169.31.1554576944.99502");
    INSERTTAG("SeriesDate",                    tagSeriesDate,                    "20190131");
    INSERTTAG("SeriesTime",                    tagSeriesTime,                    "134112.100000");
    INSERTTAG("AcquisitionDate",               tagAcquisitionDate,               "20190131");
    INSERTTAG("AcquisitionTime",               tagAcquisitionTime,               "134112.100000");
    INSERTTAG("SequenceName",                  tagSequenceName,                  "*se2d1");
    INSERTTAG("ScanningSequence",              tagScanningSequence,              "SE");
    INSERTTAG("SequenceVariant",               tagSequenceVariant,               "SP\OSP");
    INSERTTAG("MagneticFieldStrength",         tagMagneticFieldStrength,         "1.5");
    INSERTTAG("StationName",                   tagStationName,                   "MR20492");
    INSERTTAG("DeviceSerialNumber",            tagDeviceSerialNumber,            "12345");
    INSERTTAG("DeviceUID",                     tagDeviceUID,                     "1.2.276.0.7230010.3.1.4.8323329.22517.1564764826.40200");
    INSERTTAG("SoftwareVersions",              tagSoftwareVersions,              "hermes MR A10");
    INSERTTAG("ContrastBolusAgent",            tagContrastBolusAgent,            "8.0 ML JUICE");
    INSERTTAG("ImageComments",                 tagImageComments,                 "Comment on image");
    INSERTTAG("SliceThickness",                tagSliceThickness,                "3");

    fprintf(fp, "\"Filename\": \"%s\"\n",originalFile.c_str());
    fprintf(fp, "}\n");

    fclose(fp);
    return true;
}


#define READTAG(TAG,VAR) if ((dcmFile.getDataset()->tagExistsWithValue(TAG)) && (!dcmFile.getDataset()->findAndGetOFString(TAG, VAR).good())) \
                         {  \
                             OFString errorStr="Unable to read tag ";\
                             errorStr.append(TAG.toString()); \
                             errorStr.append("\nReason: "); \
                             errorStr.append(dcmFile.getDataset()->findAndGetOFString(TAG, VAR).text()); \
                             writeErrorInformation(path+origFilename, errorStr); \
                             return 1; \
                         } \
                         for (size_t i=0; i<VAR.length(); i++) { if (VAR[i]==13) { VAR[i]=';'; } else { if (VAR[i]==10) { VAR[i]=' '; } } }

int main(int argc, char *argv[])
{
    if (!charsetConverter.isConversionAvailable())
    {
        std::cout << std::endl;
        std::cout << "ERROR: Characterset converter not available" << std::endl << std::endl;
        std::cout << "ERROR: Check installed libraries" << std::endl << std::endl;

        return 1;
    }

    if (argc < 2)
    {
        std::cout << std::endl;
        std::cout << "getdcmtags ver "                                      << VERSION   << std::endl;
        std::cout << "-------------------"                                  << std::endl << std::endl;
        std::cout << "Usage: [dcm file to analyze] [ip:port of bookkeeper]" << std::endl << std::endl;

        return 0;
    }

    if (argc > 2)
    {
        bookkeeperAddress=std::string(argv[2]);
    }

    OFString origFilename=OFString(argv[1]);
    OFString path="";

    size_t slashPos=origFilename.rfind("/");
    if (slashPos!=OFString_npos)
    {
        path=origFilename.substr(0,slashPos+1);
        origFilename.erase(0,slashPos+1);
    }

    DcmFileFormat dcmFile;
    OFCondition   status=dcmFile.loadFile(path+origFilename);

    if (!status.good())
    {
        OFString errorString="Unable to read DICOM file ";
        errorString.append(origFilename);
        errorString.append("\n");
        writeErrorInformation(path+origFilename, errorString);
        return 1;
    }

    READTAG(DCM_SpecificCharacterSet,          tagSpecificCharacterSet);
    READTAG(DCM_Modality,                      tagModality);
    READTAG(DCM_BodyPartExamined,              tagBodyPartExamined);
    READTAG(DCM_ProtocolName,                  tagProtocolName);
    READTAG(DCM_RetrieveAETitle,               tagRetrieveAETitle);
    READTAG(DCM_StationAETitle,                tagStationAETitle);
    READTAG(DCM_Manufacturer,                  tagManufacturer);
    READTAG(DCM_ManufacturerModelName,         tagManufacturerModelName);
    READTAG(DCM_StudyDescription,              tagStudyDescription);
    READTAG(DCM_CodeValue,                     tagCodeValue);
    READTAG(DCM_CodeMeaning,                   tagCodeMeaning);
    READTAG(DCM_SeriesDescription,             tagSeriesDescription);
    READTAG(DCM_PatientName,                   tagPatientName);
    READTAG(DCM_PatientID,                     tagPatientID);
    READTAG(DCM_PatientBirthDate,              tagPatientBirthDate);
    READTAG(DCM_PatientSex,                    tagPatientSex);
    READTAG(DCM_AccessionNumber,               tagAccessionNumber);
    READTAG(DCM_ReferringPhysicianName,        tagReferringPhysicianName);
    READTAG(DCM_StudyID,                       tagStudyID);
    READTAG(DCM_SeriesNumber,                  tagSeriesNumber);
    READTAG(DCM_SOPInstanceUID,                tagSOPInstanceUID);
    READTAG(DCM_SeriesInstanceUID,             tagSeriesInstanceUID);
    READTAG(DCM_StudyInstanceUID,              tagStudyInstanceUID);
    READTAG(DCM_SeriesDate,                    tagSeriesDate);
    READTAG(DCM_SeriesTime,                    tagSeriesTime);
    READTAG(DCM_AcquisitionDate,               tagAcquisitionDate);
    READTAG(DCM_AcquisitionTime,               tagAcquisitionTime);
    READTAG(DCM_SequenceName,                  tagSequenceName);
    READTAG(DCM_ScanningSequence,              tagScanningSequence);
    READTAG(DCM_SequenceVariant,               tagSequenceVariant);
    READTAG(DCM_MagneticFieldStrength,         tagMagneticFieldStrength);
    READTAG(DCM_StationName,                   tagStationName);
    READTAG(DCM_DeviceSerialNumber,            tagDeviceSerialNumber);
    READTAG(DCM_DeviceUID,                     tagDeviceUID);
    READTAG(DCM_SoftwareVersions,              tagSoftwareVersions);
    READTAG(DCM_ContrastBolusAgent,            tagContrastBolusAgent);
    READTAG(DCM_ImageComments,                 tagImageComments);
    READTAG(DCM_SliceThickness,                tagSliceThickness);

    isConversionNeeded=true;
    if (tagSpecificCharacterSet.compare("ISO_IR 192")==0)
    {
        // Incoming DICOM image already has UTF-8 format, conversion is not needed.
        isConversionNeeded=false;
    }

    if (!charsetConverter.selectCharacterSet(tagSpecificCharacterSet).good())
    {
        std::cout << "ERROR: Unable to perform character set conversion! " << std::endl;
        std::cout << "ERROR: Incoming charset is " << tagSpecificCharacterSet << std::endl;
        return 1;
    }

    OFString newFilename=tagSeriesInstanceUID+"#"+origFilename;

    if (rename((path+origFilename).c_str(), (path+newFilename+".dcm").c_str())!=0)
    {
        OFString errorString="Unable to rename DICOM file to ";
        errorString.append(newFilename);
        errorString.append("\n");
        writeErrorInformation(path+origFilename, errorString);
        return 1;
    }

    if (!writeTagsFile(path+newFilename,origFilename))
    {
        OFString errorString="Unable to write tagsfile file for ";
        errorString.append(newFilename);
        errorString.append("\n");
        writeErrorInformation(path+origFilename, errorString);
        return 1;
    }

    sendBookkeeperPost(newFilename, tagSOPInstanceUID, tagSeriesInstanceUID);
}
