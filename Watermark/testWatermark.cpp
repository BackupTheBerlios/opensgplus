// OpenSGPLUS Tutorial Example: Watermark embedding
//
// this example shows how to embed a watermark in a scene
// and retrieve it using the OpenSGPLUS watermark component
//
// Headers
#include <OSGConfig.h>
#include <OSGSceneFileHandler.h>
#include <OSGVRMLWriteAction.h>
#include <OSGVRMLFile.h>
#include <OSGWatermark.h>

#include <string.h>

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

// parses the command-line parameters
bool parseParams(int argc, char **argv, int &progAction, char *labelStr,
                 char *inFileName, char *outFileName)
{
    bool    paramCorrect = true;

    if(argc < 3)
    {
        FWARNING(("too few parameters\n"));
        paramCorrect = false;
    }
    else
    {
        if(strcmp(argv[1], "e") == 0)
        {
            progAction = 0;
        }
        else if(strcmp(argv[1], "r") == 0)
        {
            progAction = 1;
        }
        else
        {
            FWARNING(("need either parameter e or r\n"));
            paramCorrect = false;
        }

        if(progAction == 0)
        {
            if(argc < 5)
            {
                FWARNING(("too few parameters\n"));
                paramCorrect = false;
            }
            else
            {
                strcpy(labelStr, argv[2]);
                strcpy(inFileName, argv[3]);
                strcpy(outFileName, argv[4]);
            }
        }
        else if(progAction == 1)
        {
            strcpy(inFileName, argv[2]);
        }
    }

    if(!paramCorrect)
    {
        FLOG(("usage:\n"));
        FLOG(("WMTutorial e|r [labelStr] inputFile [outputFile]\n"));
        FLOG(("    e: embed labelStr in inputFile and write to outputFile\n"));
        FLOG(("    r: retrieve watermark from inputFile\n\n"));
    }

    return paramCorrect;
}

// load the specified file and do the watermark embedding/retrieving
int main(int argc, char **argv)
{
    int     progAction;
    char    labelStr[128];
    char    inFileName[1024];
    char    outFileName[1024];

    // OSG init
    osgInit(argc, argv);

    // parse parameters
    if(!parseParams(argc, argv, progAction, labelStr, inFileName, outFileName))
        return -1;

    // load the scene
    FLOG(("loading scene: %s\n", inFileName));

    NodePtr scene;

    scene = SceneFileHandler::the().read(inFileName);

    if(scene == NullFC)
    {
        FWARNING(("Could not load scene\n"));
        return -1;
    }

    OSGWatermark    wm;

    if(progAction == 0)
    {
        FLOG(("embedding watermark \"%s\" ...\n", labelStr));

        if(wm.embedWM(scene, labelStr))
        {
            FLOG(("watermark successful embedded\n"));
            FLOG(("writing watermarked scene to file %s", outFileName));

            // export as vrml file
            VRMLWriteAction *pWriter = VRMLWriteAction::create();
            if(!pWriter->open(outFileName))
            {
                FWARNING(("unable to open file for writing"));
            }
            else
            {
                pWriter->write(scene);
                pWriter->close();
                FLOG(("file written\n"));
            }
        }
        else
        {
            FWARNING(("watermark embedding failed\n"));
        }
    }
    else if(progAction == 1)
    {
        strcpy(labelStr, "");

        FLOG(("retrieving watermark ...\n"));

        if(wm.retrieveWM(scene, labelStr))
        {
            FLOG(("watermark found: \"%s\"\n", labelStr));
        }
        else
        {
            FWARNING(("no watermark found\n"));
            FLOG(("watermark found: \"%s\"\n", labelStr));
        }
    }

    return 0;
}
