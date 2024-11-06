//To handle all the types of the enumeration types..
#pragma once
#include "..\MainDLL.h"
class MAINDLL_API RapidEnums
{

#pragma region Handler enumeration Types
public:
	enum RAPIDHANDLERTYPE
	{
		LINE_HANDLER,
		XLINE_HANDLER,
		XRAY_HANDLER,
		LINE3D_HANDLER,
		CIRCLE3P_HANDLER,
		CIRCLE1P_HANDLER,
		ARC_HANDLER,
		POINT_HANDLER,
		POINT_HANDLER3D,
		FASTRACE_HANDLER,
		FASTRACE_HANDLER_ENDPT,
		TRIMMING_HANDLER,
		SELECTION_HANDLER,
		MOVE_HANDLER,
		MEASURE_HANDLER,
		ROTARYMEASURE_HANDLER,
		PCDMEASURE_HANDLER,
		PLANE_HANDLER,
		CYLINDER_HANDLER,
		PERIMETER_HANDLER,
		FIXEDRECT_FRAMEGRAB,
		CIRCLE_FRAMEGRAB,
		ARECT_FRAMEGRAB,
		ARC_FRAMEGRAB,
		RECT_FRAMEGRAB,
		RECT_FRAMEGRAB_ALLEDGE,
		MOUSECLICK_GRAB,
		ONE_POINTANGLE_LINE,
		ONE_POINT_OFFSET_LINE,
		DMM_HANDLER,
		RTEXT_HANDLER,
		RRECT_HANDLER,
		FOCUSBOX,
		MIDPOINT,
		PARALLEL_LINE,
		PERPENDICULAR_LINE,
		NEARESTPOINT,
		FARTHESTPOINT,
		TANGENT_ON_CIRCLE,
		PARALLEL_TANGENT,
		PERPENDICULAR_TANGENT,
		TANGENTIAL_CIRCLE,
		TANGENT_POINT_CIRCLE,
		TANGENT_ON_2CIRCLE,
		PARALLEL_TANGENT_2CIRCLE,
		POINTONCIRCLE,
		POINTONLINE,
		CIRCLETANTOCIRCLE,
		CIRCLETANTO2CIRCLE,
		CIRCLETANTO2CIRCLE1,
		CIRCLETANTOLINECIRCLE,
		CIRCLE_TANTOTRIANGLEIN,
		CIRCLE_TANTOTRIANGLEOUT,
		PARALLELARC,
		ANGLE_BISECTOR,
		SPHERE_HANDLER,
		CLOUDPOINTS_HANDLER,
		RCADZOOMIN,
		RCADZOOMOUT,
		DXFZOOMIN,
		DXFZOOMOUT,
		UCS_POINT,
		UCS_LINE,
		UCS_SHIFT,
		SHIFT_ORIGIN,
		FILLET_HANDLER,
		DXFAREA_HANDLER,
		CADALIGNMENT_1PT,
		CADALIGNMENT_2PT,
		CADALIGNMENT_1PT1LINE,
		CADALIGNMENT_2LINE,
		CADALIGNMENT_1LINE3D,
		CADALIGNMENT_1PT1LINE3D,
		SMARTMEASUREMODE_HANDLER,
		IDMEASURE_HANDLER,
		ODMEASURE_HANDLER,
		DETECTALLEDGED_HANDLER,
		FLEX_CROSSHAIR,
		SCAN_CROSSHAIR,
		SELECTPOINT_HANDLER,
		LIVESCAN_CROSSHAIR,
		CONE_HANDLER,
		CIRCLE3D_HANDLER,
		ELLIPSE3D_HANDLER,
		PARABOLA3D_HANDLER,
		HYPERBOLA3D_HANDLER,
		FOCUS_DEPTHTWOBOXHANDLER,
		FOCUS_DEPTHFOURBOXHANDLER,
		THREAD_LINEARCHANDLER,
		PCD_HANDLER,
		PCD_HANDLER_3D,
		TORUS_HANDLER,
		LINEARCLINE_HANDLER,
		DEFAULT_HANDLER,
		UCS_PLANE,
		TWOARC_HANDLER,
		TWOLINE_HANDLER,
		LINEARCFG_HANDLER,
		PPALIGNMENT_1PT,
		PPALIGNMENT_2PT,
		PPALIGNMENT_1PT1LINE,
		PROBE_HANDLER,
		ARC_TANTO_ARC_HANDLER,
		RCADFG_HANDLER,
		POLYLINECLOUDPOINTS,
		PERPENDICULAR_PLANE,
		PARALLEL_PLANE,
		PARALLEL_LINE3D,
		PERPENDICULAR_LINE3D,
		REFLECTIONHANDLER,
		ROTATIONHANDLER,
		TRANSLATIONHANDLER,
		CAD_1PT_EDITALIGN_HANDLER,
		CAD_2PT_EDITALIGN_HANDLER,
		SELECTED_TRANSFORM_HANDLER,
		SELECTED_TRANSLATE_HANDLER,
		SELECTED_ROTATE_HANDLER,
		ROTATE_HANDLER,
		TRANSFORM_HANDLER,
		TRANSLATE_HANDLER,
		RELATIVEPOINT_HANDLER,
		DEVIATION_HANDLER,
		UCS_XY,
		UCS_YZ,
		UCS_XZ,
		EDITCAD_2LINEHANDLER,
		EDITCAD_1PT1LINE_HANDLER,
		AUTOFOCUSSCANHANDLER,
		INTERSECTIONSHAPEHANDLER,
		PROFILESCANHANDLER,
		CONTOURSCAN_FGHANDLER,
		POINTFROMSHAPEHANDLER,
		PLANEFROMTWOLINEHANDLER,
		SHAPEPROJECTIONHANDLER,
		ARC_TANTO_LINE_HANDLER,
		DXF_DEVIATION_HANDLER,
		FILTERDPLANE_HANDLER,
		LINEPER2LINEONPLN_HANDLER,
		SPLINE_HANDLER,
		FILTERDLINE_HANDLER,
		FILTEREDARC_HANDLER,
		OUTERMOSTLINE_HANDLER,
		OUTERMOSTCIRCLE_HANDLER,
		WHITE_AREA_HANDLER,
		BLACK_AREA_HANDLER,
		OUTERMOSTCLOUDPOINTS_HANDLER,
		CADALIGNMENT_3PT,
		FOCUSONRIGHTCLICKHANDLER,
		CIRCLEINVOLUTE_HANDLER,
		CLOUDCOMPARISION_HANDLER,
		CONESPHEREINTERSECT_HANDLER,
		CLOUDPOINTMEASUREMENTHANDLER,
		AUTOCONTOURSCANHANDLER,	
		NEARESTPOINT3D,
		FARTHESTPOINT3D,
		ROTATIONSCAN_CIRCLE_FRAMEGRAB,
		ROTATIONSCAN_ARC_FRAMEGRAB,
		ROTATIONSCAN_ARECT_FRAMEGRAB,
		ROTATIONSCAN_RECT_FRAMEGRAB,
		ROTATIONSCAN_FIXEDRECT_FRAMEGRAB,
		PROGRAM_1PT_ALIGNMENT,
		PERPENDICULARPLANE_ONLINE,
		DOTCOUNTS_HANDLER,
		IMAGEANALYSIS_FGHANDLER,
		PROGRAM_1LN_1PTALIGNMENT,
		TRANSLATIONHANDLERPOINTTOPOINT,
		SPHEREFRAMEGRABHANDLER,
		TWOPOINTALIGNMENTHANDLER,
		COPYSHAPE_2PTALIGN,
		PROGRAM_2PT_ALIGNMENT,
		PARALLEL_LINE_FG,
		CAMPROFILE_HANDLER,
};

	enum RAPIDDRAWHANDLERTYPE
	{
		RAPIDDRAW_HANLER,
		RAPIDMEASURE_HANDLER,
		RAPIDDERIVEDSHAPE_HANDLER,
		RAPIDFRAMEGRAB_HANDLER,
		CADALIGNMENT_HANDLER,
		DXFZOOMINOUT_HANDLER,
		RCADZOOMINOUT_HANDLER,
		RAPIDDEFAULT_HANLDER,
	};

	enum CADALIGNMENTTYPE
	{
		ALIGN_ONEPOINT,
		ALIGN_TWOPOINT,
		ALIGN_ONEPTLINE,
		ALIGN_TWOLINE
	};

	enum SMARTMEASUREMENTMODE
	{
		ANGLEMEASUREMENT,
		ANGLEMEASUREWITH_XAXIS,
		ANGLEMEASUREWITH_YAXIS,
		RADIUSMEASUREMENT,
		DIAMEASUREMENT,
		STEPMEASUREMENT_3LINES,
		STEPMEASUREMENT_3LINES1,
		STEPMEASUREMENT_4LINES,
		RECANGLEMEASUREMENT,
		CHAMFERMEASUREMENT_LINE,
		CHAMFERMEASUREMENT_ARC,
		GROOVEMEASUREMENTS_ONEFRAME,
		GROOVEMEASUREMENT,
		LINEARCFG,
		SPLINEMEASUREMENT,
		DEFAULTTYPE,
		STEPMEASUREMENT_3LINESWITHAXIS,
		STEPMEASUREMENT_4LINESWITHAXIS,
		PARALLELRUNOUT, 
		INTERSECTIONRUNOUT,
		POINTANGLE,
		POINTLINEWIDTH,
		LINELINEWIDTH,
		LINEARCWIDTH,
		POINTARCWIDTH,
		ARCARCWIDTH,
	};
#pragma endregion

#pragma region FrameGrab Types And Probe Dir
	enum RAPIDFGACTIONTYPE
	{
		POINT_FOR_SHAPE,
		FLEXIBLECROSSHAIR,
		ARCFRAMEGRAB,
		CIRCLEFRAMEGRAB,
		RECTANGLEFRAMEGRAB,
		ANGULARRECTANGLEFRAMEGRAB,
		FIXEDRECTANGLE,
		EDGE_MOUSECLICK,
		PROBE_POINT,
		FOCUS_DEPTH,
		FOCUS_DEPTH_DRO,
		FOCUS_SCAN,
		FOCUS_CONTOURSCAN,
		FOCUS_DEPTH_2BOX,
		FOCUS_DEPTH_4BOX,
		SCANCROSSHAIR,
		LIVESCANCROSSHAIR,
		SELECTPOINTCROSSHAIR,
		IDMEASUREMENT,
		ODMEASUREMENT,
		DETECT_ALLEDGES,
		RECTANGLEFRAMEGRAB_ALLEDGES,
		CONTOURSCAN_FG,
		RCADPOINTS_FG,
		ROTATION_ARCFRAMEGRAB,
		ROTATION_CIRCLEFRAMEGRAB,
		ROTATION_RECTANGLEFRAMEGRAB,
		ROTATION_ANGULARRECTANGLEFRAMEGRAB,
		ROTATIONSCAN_FIXEDRECTANGLE,
	};

	enum PROBEDIRECTION
	{
		XRIGHT2LEFT,
		XLEFT2RIGHT,
		YTOP2BOTTOM,
		YBOTTOM2TOP,
		ZTOP2BOTTOM,
		ZBOTTOM2TOP,
		RCCW,
		RCW,
	};

	enum PROBEPOSITION
	{
		LEFT,
		RIGHT,
		BOTTOM,	
		TOP,
	};

#pragma endregion
	
#pragma region Projection Tpyes
	enum RAPIDPROJECTIONTYPE
	{
		XY,
		YZ,
		XZ,
		XZm, //X axis positive and Z is negative used with reflector mirror
		YZm, //Y axis positive and Z is negative used with reflector mirror
	}; 
#pragma endregion

#pragma region Projection Tpyes
enum MCSTYPE
{
	MCSXY,
	MCSYZ,
	MCSXZ,
	MCSNEGYZ,
	MCSNEGXZ,
}; 
#pragma endregion

#pragma region Cursor Tpye Enumerations
	enum RWINDOW_CURSORTYPE
	{
		CURSOR_DEFAULT,
		CURSOR_CROSS,
		CURSOR_ARCFG,	  
		CURSOR_CIRCLEFG,
		CURSOR_ANGRECTFG,
		CURSOR_RECTFG,
		CURSOR_NULL,
		CURSOR_TRIM,
		CURSOR_PANHOLD,		
		CURSOR_PANRELEASE,		
	};
#pragma endregion

#pragma region Measurement Units
	enum RAPIDUNITS
	{
		MM,
		INCHES
	};
#pragma endregion

#pragma region Lense Type
	enum RAPIDLENSETYPE
	{
		LENSE_1X,
		LENSE_2X,
		LENSE_5X,
	};
#pragma endregion

#pragma region MachineTypes
	enum RAPIDMACHINETYPE
	{
		FULLCNC,
		AUTOFOCUS,
		HORIZONTAL,
		ROTARY_DEL_FI,
		ROTARY_BOSCH_FI,
		ONE_SHOT,
		DSP,
		MAGNIFIER_DSP,
		NORMAL_ONE_SHOT,
		HORIZONTAL_DSP,
		PROBE_ONLY,
		HEIGHT_GAUGE,
		HEIGHTGAUGE_VISION,
		TOUCHTYPE_CONTOURTRACER,
		TIS,
		HOBCHECKER,
		ONLINESYSTEM,
		VISIONHEADONLY,
	};
#pragma endregion

#pragma region CurrentFocus Mode.
	enum RAPIDFOCUSMODE
	{
		FOCUSDEPTH_MODE,
		TWOBOXDEPTH_MODE,
		FOURBOXDEPTH_MODE,
		FOCUSSCAN_MODE,
		CONTOOURSCAN_MODE,
		HOMING_MODE,
		CALIBRATION_MODE,
		AUTOFOCUSSCANMODE,
		NOTHING,
		FOCUS_VBLOCK_CALLIBRATION,
		CONTOURFRAMEGRAB,
		SCANFORRIGHTCLICK,
		PROFILESCAN_FOCUS,
		AUTOCONTOURSCANMODE,
		REFERENCEDOT_SECOND,
		PROBEVISIONOFFSETFOCUS,
	};
#pragma endregion

#pragma region CurrentProfileScan Mode.
	enum RAPIDPROFILESCANMODE
	{
		PROFILESCAN_NORMAL,
		PROFILESCAN_AUTOTHREAD,
		PROFILESCAN_LINEARCACT,
		PROFILESCAN_NOTHING
	};
#pragma endregion

#pragma region ShapeTypes
	enum SHAPETYPE
	{
		LINE,
		XRAY,
		XLINE,
		LINE3D,
		CIRCLE,
		ARC,
		RPOINT,
		RTEXT,
		PERIMETER,
		POLYLINE,
		DEPTHSHAPE,
		RPOINT3D,
		CLOUDPOINTS,
		SPHERE,
		PLANE,
		CYLINDER,
		CONE,
		CIRCLE3D,
		ARC3D,
		ELLIPSE3D,
		PARABOLA3D,
		HYPERBOLA3D,
		DUMMY,
		TORUS,
		TWOARC,
		TWOLINE,
		DEFAULTSHAPE,
		RELATIVEPOINT,
		INTERSECTIONSHAPE,
		TRIANGULARSURFACE,
		SPLINE,
		AREA,
		CIRCLEINVOLUTE,
		AUTOFOCUSSURFACE,
		AUTOCONTOURLINE,
		DOTSCOUNT,
		XLINE3D,
		CAMPROFILE,
	};
#pragma endregion

#pragma region MeasurementTypes
	enum MEASUREMENTTYPE
	{
		DIM_POINTTOPOINTDISTANCE,
		DIM_POINTTOPOINTDISTANCE_LANG,
		DIM_POINTTOLINEDISTANCE,
		DIM_POINTTOCIRCLEDISTANCE,
		DIM_DIGITALMICROMETER,
		DIM_LINETOLINEANGLE,
		DIM_LINETOLINEDISTANCE,
		DIM_CIRCLETOLINEDISTANCE,
		DIM_CIRCLETOCIRCLEDISTANCE,
		DIM_PLANETOCIRCLE3DDISTANCE,
		
		DIM_POINTCOORDINATE,
		DIM_ORDINATEX,
		DIM_ORDINATEY,
		DIM_ORDINATEZ,
		
		DIM_LINEPARALLELISM,
		DIM_LINEPERPENDICULARITY,
		DIM_LINEANGULARITY,
		DIM_LINESTRAIGHTNESS,
		DIM_LINEWIDTH,
		DIM_LINESLOPE,
		
		DIM_DIAMETER2D,
		DIM_RADIUS2D,
		DIM_CIRCULARITY,
		DIM_CONCENTRICITY,
		DIM_CIRCLEMIC,
		DIM_CIRCLEMCC,
		DIM_ARCLENGTH,
		DIM_ARCANGLE,	
		DIM_TRUEPOSITION_R,
		DIM_TRUEPOSITION_D,
		DIM_TRUEPOSITIONX,
		DIM_TRUEPOSITIONY,
		
		DIM_DEPTH,
		DIM_ROTARYANGLEMEASURE,
		DIM_THREAD,
		DIM_PCDANGLE,
		DIM_PCDOFFSET,
	
		DIM_POINTTOPOINTDISTANCE3D,
		DIM_POINTTOLINEDISTANCE3D,
		DIM_POINTTOPLANEDISTANCE,
		DIM_POINTTOCYLINDERDISTANCE,
		DIM_POINTTOSPHEREDISTANCE,
		DIM_POINTTOCIRCLE3DDISTANCE,

		DIM_LINETOLINEDISTANCE3D,
		DIM_LINETOPLANEDISTANCE,
		DIM_LINETOCYLINDERDISTANCE,
		DIM_LINETOSPHEREDISTANCE,
		DIM_LINETOCIRCLE3DDISTANCE,

		DIM_PLANETOPLANEDISTANCE,
		DIM_PLANETOCYLINDERDISTANCE,
		DIM_PLANETOSPHEREDISTANCE,

		DIM_CYLINDERTOCYLINDERDISTANCE,
		DIM_CYLINDERTOSPHEREDISTANCE,

		DIM_SPHERETOSPHEREDISTANCE,

		DIM_DIAMETER3D,
		DIM_RADIUS3D,

		DIM_CYLINDRICITY,
		DIM_CYLINDERMIC,
		DIM_CYLINDERMCC,
		DIM_CYLINDERPERPENDICULARITY,
		DIM_CYLINDERANGULARITY,
		DIM_CYLINDERCOAXIALITY,

		DIM_SPHERICITY,
		DIM_SPHEREMIC,
		DIM_SPHEREMCC,

		DIM_PLANEFLATNESS,
		DIM_PLANEPARALLELISM,
		DIM_PLANEPERPENDICULARITY,
		DIM_PLANEANGULARITY,

		DIM_LINE3DSTRAIGHTNESS,
		DIM_LINE3DPARALLELISM,
		DIM_LINE3DPERPENDICULARITY,
		DIM_LINE3DANGULARITY,

		DIM_PERIMETER_DIAMETER,
		DIM_PERIMETER_PERIMETER,
		DIM_PERIMETER_AREA,
		DIM_CIRCLE3DTOCIRCLE3DDISTANCE,
		DIM_SPHERETOCIRCLE3DDISTANCE,
		DIM_CONEANGLE,
		DIM_POINTTOCONEDISTANCE,
		DIM_LINETOCONEDISTANCE,
		DIM_PLANETOCONEDISTANCE,
		DIM_CYLINDERTOCONEDISTANCE,
		DIM_CONETOCONEDISTANCE,
		DIM_SPHERETOCONEDISTANCE,
		DIM_CIRCLE3DTOCONEDISTANCE,
		DIM_AXISLENGTH,
		DIM_POINTTOCONICS3DDISTANCE,
		DIM_CONETOCONICS3DDISTANCE,
		DIM_CYLINDERTOCONICS3DDISTANCE,
		DIM_LINETOCONICS3DDISTANCE,
		DIM_SPHERETOCONICS3DDISTANCE,
		DIM_PLANETOCONICS3DDISTANCE,
		DIM_CONICS3DTOCONICS3DDISTANCE,
		
		DIM_SPLINECOUNT,
		DIM_SPLINECIRCLEMIC,
		DIM_SPLINECIRCLEMCC,

		DIM_GROOVEWIDTH,
		DIM_GROOVEDEPTH,
		DIM_GROOVEANGLE,
		DEFAULTMEASURE,
		DIM_CIRCLEDEVIATION_MEASURE,
		DIM_INTERSECTIONSHAPETOPLANEDISTANCE,
		DIM_CONCYLTOPLANEDISTANCE,

		DIM_CONICALITY,
		DIM_CONEPERPENDICULARITY,
		DIM_CONEANGULARITY,
		DIM_CONECOAXIALITY,
		DIM_CLOUDPOINTDEPTH,
		DIMTOTAL_LENGTH,
		DIMAREAMEASUREMENT,

		DIM_SURFACEROUGHNESS_SQ,
		DIM_SURFACEROUGHNESS_SA,
		DIM_SURFACEROUGHNESS_SP,
		DIM_SURFACEROUGHNESS_SV,
		DIM_SURFACEROUGHNESS_SZ,
		DIM_SURFACEROUGHNESS_SSK,
		DIM_SURFACEROUGHNESS_SKU,

		DIM_CYLINDERPARALLELISM,
		DIM_CONEPARALLELISM,

		DIM_3DTRUEPOSITION_R,
		DIM_3DTRUEPOSITION_D,

		DIM_TRUEPOSITIONPOINT_R,
		DIM_TRUEPOSITIONPOINT_D,
		DIM_TRUEPOSITIONPOINTX,
		DIM_TRUEPOSITIONPOINTY,
		DIM_LINESTRAIGHTNESSFILTERED,
		DIM_LINE3DSTRAIGHTNESSFILTERED,
		DIM_PLANEFLATNESSFILTERED,
		DIM_CYLINDRICITYFILTERED,
		DIM_POINTTOPOINTDISTANCEONAXIS,
		DIM_RUNOUTMEASUREMENTFARTHEST,
		DIM_RUNOUTMEASUREMENTNEAREST, 
		DIM_INTERSECTIONRUNOUTFARTHEST,
		DIM_INTERSECTIONRUNOUTNEAREST,  
		DIM_ARC2ARCWIDTHMEASUREMENT,
		DIM_LINE2ARCWIDTHMEASUREMENT,  
		DIM_POINT2ARCWIDTHMEASUREMENT,
		DIM_LINETOLINERAYANGLE,
		DIM_3DTRUEPOSITIONPOINT_R,
		DIM_3DTRUEPOSITIONPOINT_D,
		DIM_GEOMETRICCUTTINGDIAMETER,
		DIM_MICROMETRICCUTTINGDIAMETER,
		DIM_DIST1,
		DIM_DIST2,
		DIM_PROFILE,
		DIM_CONCENTRICITY_D,
		DIM_AXIALRUNOUT
	};
#pragma endregion


#pragma region Action Types
	enum ACTIONTYPE
	{
		ADDPOINTACTION,
		ADDSHAPEACTION,
		SELECTSHAPEACTION,
		COPYSHAPEACTION,
		MOVESHAPEACTION,
		ADDDIMACTION,
		ADDUCSACTION,
		CHANGEUCSACTION,
		DELETEMEASUREACTION,
		DELETESHAPEACTION,
		DELETEUCSACTION,
		ADDPROBEPATHACTION,
		ADDTHREADMEASUREACTION,
		ADDTHREADPOINTACTION,
		ADDLINEARCPFACTION,
		MERGESHAPEACTION,
		PERIMETERACTION,
		TRIMSHAPEACTION,
		CAD_1PT_ALIGNACTION,
		CAD_2PT_ALIGNACTION,
		EDIT_CAD_1PT_ALIGNACTION,
		EDIT_CAD_2PT_ALIGNACTION,
		AUTOALIGNACTION,
		DEVIATIONACTION,
		EDIT_CAD_2LN_ALIGNACTION,
		REFLECTIONACTION,
		TRANSLATIONACTION,
		ROTATIONACTION,
		EDIT_CAD_1PT1LN_ALIGNACTION,
		DXF_DEVIATIONACTION,
		IMAGE_ACTION,
		INTERPOLATION_ACTION,
		LINEAR_INTERPOLATION_ACTION,
		PROFILE_SCAN_ACTION,
		AREA_ACTION,
		EDITCADACTION,
		CAD_1PT1LN_ALIGNACTION,
		CAD_2LN_ALIGNACTION,
		CLOUD_COMPARISION_ACTION,
		ADDSHAPEPOINT_ACTION,
		LOCALISEDCORRECTION,
	};
#pragma endregion

#pragma region For Internal Shape Type
	enum CIRCLETYPE
	{
		CIRCLE_WITHCENTER,
		TWOPTCIRCLE,
		TRHEEPTCIRCLE,
		PCDCIRCLE,
		CIRCLE3DCIR,
		ARC3DCIR,
		ARC_MIDDLE,
		ARC_ENDPOINT,
		PARALLELARCCIR,
		TANCIRCLETO_2CIRCLES_IN,
		TANCIRCLETO_2CIRCLES_OUT,
		TANCIRCLETO_CIRCLE,
		TANCIRCLETO_2LINES,
		TANCIRCLETO_TRIANG_IN,
		TANCIRCLETO_TRIANG_OUT,
		TANCIRCLETO_LINECIRCLE,
		LINEARCLINE_CIRCLE,
		ARC_TANGENT_2_LINES_CIRCLE,
		ARC_INTWOARC,
		ARC_ARC_SIBLINGS,
		ARC_TAN_TO_ARC,
		ARC_TAN_TO_LINE,
		CIRCLEFROMCLOUDPNTS,
		CONESPHEREINTERSECT,
		PROJECTEDCIRCLE,
	};

	enum LINETYPE
	{
		FINITELINE,
		INFINITELINE,
		RAYLINE,
		FINITELINE3D,
		PARALLEL_LINEL,
		PERPENDICULAR_LINEL,
		TANGENT_ON_CIRCLEL,
		PARALLEL_TANGENTL,
		PERPENDICULAR_TANGENTL,
		TANGENT_THRU_POINT,
		TANGENT_ON_2_CIRCLE,
		ANGLE_BISECTORL,
		PROJECTED_LINE_OF_PLANEL,		//added by vinod For projected_lineof_plane
		ONEPT_ANGLELINE,
		ONEPT_OFFSETLINE,
		PARALLEL_TANGENT2TWOCIRLE,
		INTERSECTIONLINE3D,
		LINEINTWOARC,
		LINEINTWOLINE,
		PARALLELLINE3D,
		PERPENDICULARLINE3D,
		PROJECTIONLINE3D,
		LINEPERPENDICULAR2LINEONPLANE,
		LINEFROMCLOUDPNTS,
		MULTIPOINTSLINEPARALLEL2LINE,
		INFINITELINE3D,
		REFLECTEDLINE,
	};

	enum PLANETYPE
	{
		SIMPLE_PLANE,
		PARALLELPLANE,
		PERPENDICULARPLANE,
		PLANETHROUGHTWOLINE,
		FILTEREDPLANE,
		PLANEPERPENDICULAR2LINE,
	};

	enum CLOUDPOINTSTYPE
	{
		DEFAULT,
		INTERSECTIONSHAPEPROJECTION,
		CLOUDPOINTSPROJECTION,
	};

#pragma endregion

#pragma region Fixed Shape Type
	enum FIXEDSHAPETYPE // Shape type..
	{
		FLINE,
		FCIRCLE,
		FTEXT
	}FShapeType;

#pragma endregion

#pragma region Right Click Type For Measure Handler
	enum CURRENTRIGHTCLICKEDSHAPE
	{
		RIGHTCLICKONPOINT,
		RIGHTCLICKONLINE,
		RIGHTCLICKONCIRCLE,
		RIGHTCLICKONLINE3D,
		RIGHTCLICKONCYLINDER,
		RIGHTCLICKONPLANE,
		RIGHTCLICKONSPHERE,
		RIGHTCLICKONPERIMETER,
		RIGHTCLICKNOTHING,
		RIGHTCLICKONCONE,
		RIGHTCLICKONCIRCLE3D,
		RIGHTCLICKONCLOUDPOINTS,
	}; 
#pragma endregion


#pragma region MachineGotoType Handling
	enum MACHINEGOTOTYPE
	{
		PROFILESCANGOTO,
		SAOICELLGOTO,
		RCADWINDOWGOTO,
		AUTOMEASUREMODEGOTO,
		GRIDIMAGE,
		REPEATAUTOMEASUREGOTO,
		REPEATAUTOMEASUREGOTO_AUTOFOCUS,
		DONOTHING,
		SPHERVISIONCALIBRATION,
	};

#pragma region UcsMode Handling
	enum UCSMODE
	{
		MCSMODE,
		UCSPOINTALIGN,
		UCSLINEALIGN,
		UCSBASEPLANEALIGN,
	}; 
#pragma endregion

	#pragma region Enum for Messagebox Handling
	enum MSGBOXTYPE
	{	
		MSG_YES_NO,
		MSG_OK,
		MSG_OK_CANCEL,
		MSG_YES_NO_CANCEL,	
	}; 

	enum MSGBOXICONTYPE
	{
		MSG_EXCLAMATION,
		MSG_ERROR,
		MSG_INFORMATION,
		MSG_NONE,
		MSG_QUESTION,
		MSG_STOP,
		MSG_WARNING,
	};
	
	enum PROBETYPE
	{
		SIMPLE_PROBE,
		X_AXISTPROBE,
		Y_AXISTPROBE,
		STARPROBE,
		GENERICPROBE,
	};

#pragma endregion

#pragma region Enum for PartProgram parameter
	enum PROGRAM_PARAM
	{
		PP_NULL,
		DEVIATION,
		RELATIVE_PTOFFSET,
		DEVIATION_RELATIVE_PT,
	};
#pragma endregion

#pragma region Enum for HeightGauge
	enum STYLUSTYPE
	{
		ALONG_XAXIS,
		ALONG_YAXIS,
		ALONG_ZAXIS,
	};
#pragma endregion
};