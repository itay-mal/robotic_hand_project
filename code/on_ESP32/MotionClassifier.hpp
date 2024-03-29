#ifndef MotionClassifier
#define MotionClassifier
#include "math_helpers.cpp"

#define NUM_FETURES 6  // accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z
#define NUM_CLASSES 5  // rest, up, down, forward, back

/******** trained params ************/
const float GRU0_KERNEL[6][30] = {-0.07386524, -0.381935  , -0.13819417,  0.12401052, -0.8469981 ,
                            -0.11474337,  0.14439572,  0.68886787, -0.18585722,  0.04070459,
                            0.26943806,  0.24287698, -1.30369   ,  0.23181458,  0.35334542,
                            0.0823829 , -0.41643625,  0.19776228,  0.09342036, -0.60728896,
                            0.04334562,  0.43849242, -1.2782091 , -0.18450269, -0.23263131,
                            0.15835208,  0.38127762, -0.48940486, -0.57730645, -0.86388063,
                            0.25084186,  0.06985164, -0.55714005, -0.15118502, -0.308736  ,
                            0.19356194,  0.21364976, -0.13171847,  0.36750212,  0.19732508,
                            0.15852202, -0.17324284, -0.13682173,  0.10820474,  0.375183  ,
                            -0.21300662, -0.04609125,  0.09886084,  0.34527794,  0.00153373,
                            0.13797387, -0.10219389, -0.1161502 ,  0.36862487,  0.01255223,
                            -0.25842986,  0.0852121 , -0.07548164,  0.11782532, -0.14228836,
                            0.01601967, -0.15078335,  0.42906535, -0.25078025,  0.03700012,
                            -0.6528442 , -0.5337053 , -0.45815805,  0.18274014, -0.32370734,
                            -0.12763943,  0.5460452 ,  0.16710259,  0.08552779, -0.04461037,
                            -0.3420871 , -0.12008898,  0.06009861,  0.1208166 ,  0.22871985,
                            0.3056765 ,  0.03870973, -0.16967596, -0.36862826, -0.10333359,
                            -0.55823535, -0.6035786 , -0.44239247, -0.38686702,  0.12146933,
                            0.5071437 ,  0.33123347,  0.10848632,  0.00742129, -0.74949867,
                            0.08782335,  1.037331  ,  0.25117034, -0.33998293,  0.49202713,
                            -0.48249424, -0.16977605, -0.7018593 , -0.24025366, -0.27967793,
                            -0.6285185 ,  0.41803202,  0.09290087, -0.29560313,  0.5008331 ,
                            0.13327554, -0.31109512, -0.16752322, -0.1546875 ,  0.14715573,
                            -0.11674691,  0.30318967, -0.14596125, -0.02653846,  0.48203152,
                            0.76511717,  0.8153934 ,  0.2557308 ,  0.4705493 , -0.0988984 ,
                            0.15373574, -0.44072047, -0.47507167,  0.19546013, -0.6693386 ,
                            -0.45467556,  0.18065941, -0.55294746, -0.48205623,  0.34504163,
                            -0.13194683,  0.98787177,  0.02505996,  0.6062585 ,  0.88160336,
                            0.4137287 , -0.6088066 ,  0.1285756 , -0.4197855 ,  0.32017472,
                            -0.22202407, -0.8450644 , -0.7272508 , -0.6731188 ,  0.46054026,
                            1.0473251 ,  0.552197  ,  0.19383316,  0.725445  ,  0.67988616,
                            0.8482298 ,  0.650337  , -0.37210426, -0.44989032, -0.08723757,
                            0.5191316 ,  0.01393396,  1.042239  , -0.37968683,  0.13594052,
                            -0.37188014, -0.5117015 , -0.39961195,  0.66722107,  0.17919038,
                            -0.54389274,  0.25936723,  1.0545367 , -0.21161938, -0.39755642,
                            -0.30334988,  0.5604498 , -1.085228  , -0.25658605,  0.19367816};
const float GRU0_RECURRENT_KERNEL[10][30] = {0.2305233 , -0.19349042,  0.09297419,  0.03644874,  0.49724376,
                                       0.4700204 ,  0.01200764, -0.70014375,  0.0644481 ,  0.41717643,
                                       0.00174687,  0.00445262,  0.3216273 , -0.40925542, -0.00405546,
                                       -0.06063802,  0.7207659 , -0.8155512 ,  0.21523108,  0.5530314 ,
                                       -0.08418056, -0.19952153, -0.8134521 , -0.03362409,  0.38250506,
                                       -0.12860924, -0.79685855, -0.5099881 , -0.2128711 ,  0.66263413,
                                       0.12270321, -0.5018415 ,  0.270778  , -0.21439633,  0.22580191,
                                       -0.2084921 ,  0.12480882, -0.31140393, -0.4063757 ,  0.07476262,
                                       -0.26774544, -0.20630476,  0.1702779 , -0.1625598 ,  0.00513727,
                                       -0.05639528,  0.00913378, -0.35831007, -0.4526187 , -0.40710345,
                                       0.26811722,  0.04891435, -0.00652021, -0.02041882, -0.35065722,
                                       0.01408034,  0.44292414, -0.01119948,  0.4002052 , -0.08094913,
                                       0.11987878,  0.28573313,  0.11758211, -0.3134534 , -0.08982132,
                                       0.31740922, -0.6418022 , -0.3379521 ,  0.26289454, -0.5266717 ,
                                       -0.01299749, -0.20650117,  0.47946057, -0.01098321, -0.47008505,
                                       0.22414193,  0.03952773, -0.10955758, -0.26017928, -0.13222173,
                                       0.4084335 ,  0.21206069,  0.7080131 , -0.30923462, -0.24106652,
                                       0.17797449, -0.023896  ,  0.10863736,  0.17848024, -0.2029759,
                                       -0.30977458,  0.80285126, -0.2373684 ,  0.34353754,  0.3364863 ,
                                       -0.135092  ,  0.04324148,  0.45049393, -0.17697814, -0.06008298,
                                       0.26397598,  0.08006576,  0.17083262,  0.11669831,  0.19228216,
                                       0.35381192,  0.11658008,  0.18793498,  0.2788776 , -0.0691961 ,
                                       -0.19296053, -0.35791135,  0.19366539,  0.1110355 ,  0.1582754 ,
                                       0.20578355, -0.38157117,  0.34832555, -0.06013647,  0.31769112,
                                       0.26543868,  0.5334951 , -1.0433149 ,  0.372984  ,  0.16398941,
                                       0.15508166, -0.07561241,  0.00845528,  0.02692853, -0.10799266,
                                       -0.00365185, -0.00150333,  0.36002657, -0.27718574,  0.01138709,
                                       0.28453624,  0.20438474,  0.28158173,  0.20592771,  0.4383849 ,
                                       -0.01025691, -0.13821559,  0.13554215,  0.07233588,  0.03818523,
                                       -0.12751338, -0.19877307,  0.23570001, -0.29935136,  0.04216991,
                                       0.27235678, -0.5134881 , -0.00443725, -0.17876214, -0.10248612,
                                       0.10560521,  0.15880752,  0.34025598,  0.25594994, -0.08218647,
                                       -0.05922586, -0.32251668, -0.2882723 ,  0.14638747, -0.18475877,
                                       -0.05228329, -0.17982931, -0.1627465 , -0.65855277, -0.27080116,
                                       -0.06872264, -0.08083578, -0.05393856, -0.39763215, -0.25237447,
                                       -0.20565769,  0.2607616 , -0.03630922,  0.55258536, -0.22371739,
                                       -0.3511814 ,  0.33092803, -0.10225584, -0.05918171, -0.01664555,
                                       -0.14802627, -0.8121108 ,  0.01378036, -0.17154327,  0.03400051,
                                       0.19257101, -0.03444674,  0.18943842,  0.14569099, -0.2582265 ,
                                       -0.0087291 , -0.88886875,  0.5100689 ,  0.09788607, -0.10798375,
                                       -0.3140033 , -0.23263788,  0.05989322, -0.09787174,  0.03937409,
                                       0.11059173,  0.5428892 ,  0.0350284 ,  0.20925486, -0.7004424,
                                       -0.17515126,  0.52237844, -0.25203693,  0.12068629, -0.13273397,
                                       -0.26167563,  0.32796943,  0.3885795 ,  0.46404353,  0.21320662,
                                       0.04531422, -0.2323383 ,  0.07618959, -0.30485708,  0.14584854,
                                       -0.07672551,  0.3029077 ,  0.2805316 , -0.23074219,  0.08351288,
                                       0.01335856,  0.42864904,  0.27744776, -0.08479127, -0.23086959,
                                       0.23937862, -0.23944642,  0.47475153,  0.66176546, -0.12128798,
                                       -0.07661157,  0.7202916 , -0.4576874 ,  0.10388719,  0.46175498,
                                       0.17820893,  0.15764716,  0.37767708,  0.05652266, -0.11268916,
                                       0.4266187 ,  0.12959218,  0.53542227,  0.29762554, -0.04768341,
                                       0.14716403, -0.3286192 , -0.2609774 ,  0.3306126 ,  0.2103131 ,
                                       -0.52496797,  0.10782406,  0.33553746,  0.02976944, -0.1501264 ,
                                       0.41395038, -0.15584019,  0.50641084,  0.12896046,  0.10390235,
                                       0.13320604, -1.0563506 ,  0.31367773,  0.4240752 ,  0.29827684,
                                       0.23122542,  0.6036514 , -0.38104177,  0.5231227 ,  0.6385496 ,
                                       -0.34989482, -0.32727075, -0.13247305, -0.14557488,  0.00982756,
                                       -0.19895397,  0.7381871 , -0.43238655, -0.11875691, -0.17575082,
                                       0.4438298 , -0.20310245, -0.07909958, -0.36553472, -0.25266093,
                                       -0.4060934 , -0.46506545, -0.22422197, -0.01649249,  0.1536952};
const float GRU0_BIAS[2][30] ={-0.00683994,  0.34898096, -0.4007956 ,  0.0717561 ,  0.03940834,
                         0.18533315,  0.06993815,  0.07695332, -0.05041093, -0.01155871,
                         0.14322202,  0.12409262,  0.21973655,  0.13193023,  0.08804885,
                         0.15887247,  0.05216301,  0.16155179,  0.25056896,  0.22769298,
                         0.1455479 , -0.00197712, -0.03291975,  0.10507752,  0.12566745,
                         0.10323124, -0.08694476,  0.13529348, -0.21095648,  0.12286269,
                         -0.00683994,  0.34898096, -0.4007956 ,  0.0717561 ,  0.03940834,
                         0.18533315,  0.06993815,  0.07695332, -0.05041093, -0.01155871,
                         0.14322202,  0.12409262,  0.21973655,  0.13193023,  0.08804885,
                         0.15887247,  0.05216301,  0.16155179,  0.25056896,  0.22769298,
                         -0.15223236, -0.04688281,  0.0562954 ,  0.1291623 ,  0.12906346,
                         0.1380234 , -0.2999485 ,  0.12457279, -0.21209341,  0.15763068};
float GRU_0_STATE[10] = {0};


const float GRU1_KERNEL[10][15] = {};
const float GRU1_RECURRENT_KERNEL[5][15] = {};
const float GRU1_BIAS[2][15] = {};
float GRU1_STATE[5] = {0};

const float DENSE[5][5] = {};
/***********************************/





#endif