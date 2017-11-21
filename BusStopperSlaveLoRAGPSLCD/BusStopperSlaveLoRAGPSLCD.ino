#include <Adafruit_GPS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <GPSCoor.h>

#include <SPI.h>
#include <RH_RF95.h>
#include <elapsedMillis.h>
#include <LiquidCrystal_I2C.h>

#define GPS_STATION_DISTANCE_RANGE 10 // 10m radius from bus station
#define d2r 0.01745329251994329576923690768489
#define GPS_STATUS_ADDRESS 8 // connects to LORA; 0 - NO GPGS / in transit ; 1 - GPS found; bus stop near by
// bus stations
#define NUMBER_OF_STATIONS 508
// GPSCoor(latitude, longitude)
GPSCoor busstops[NUMBER_OF_STATIONS] = {
 GPSCoor(1.2912312, 103.831232), GPSCoor(1.312312, 103.631232), GPSCoor(1.32312, 103.7443), GPSCoor(1.43662, 103.769947),
 GPSCoor(1.380176, 103.893202), GPSCoor(1.321814, 103.892672), GPSCoor(1.41943, 103.667872), GPSCoor(1.291372, 103.718769),
 GPSCoor(1.391395, 103.617939), GPSCoor(1.250771, 103.627931), GPSCoor(1.439845, 103.632861), GPSCoor(1.45385, 103.667209),
 GPSCoor(1.420247, 103.629918), GPSCoor(1.336267, 103.817817), GPSCoor(1.264075, 103.789775), GPSCoor(1.380773, 103.714552),
 GPSCoor(1.45593, 103.846789), GPSCoor(1.385947, 103.709641), GPSCoor(1.249298, 103.989364), GPSCoor(1.332155, 103.829551),
 GPSCoor(1.412448, 103.936573), GPSCoor(1.357006, 103.813443), GPSCoor(1.358374, 103.732412), GPSCoor(1.425449, 103.987161),
 GPSCoor(1.405332, 103.689406), GPSCoor(1.283718, 103.624061), GPSCoor(1.376483, 103.785785), GPSCoor(1.241127, 103.741746),
 GPSCoor(1.326587, 103.882528), GPSCoor(1.373476, 104.007356), GPSCoor(1.286127, 103.677339), GPSCoor(1.317508, 103.789502),
 GPSCoor(1.456746, 103.996307), GPSCoor(1.350352, 103.710646), GPSCoor(1.303748, 103.857918), GPSCoor(1.390329, 104.030651),
 GPSCoor(1.249151, 103.699643), GPSCoor(1.378449, 104.023696), GPSCoor(1.385033, 103.988497), GPSCoor(1.304126, 103.823259),
 GPSCoor(1.340654, 103.703476), GPSCoor(1.365062, 103.802065), GPSCoor(1.370151, 103.86238), GPSCoor(1.280228, 103.967576), 
 GPSCoor(1.356788, 103.779449), GPSCoor(1.247345, 103.786428), GPSCoor(1.266389, 103.844946), GPSCoor(1.260891, 103.715978),
 GPSCoor(1.364186, 103.939702), GPSCoor(1.411259, 103.929133), GPSCoor(1.29402, 103.713418), GPSCoor(1.383801, 103.753284),
 GPSCoor(1.285731, 104.023402), GPSCoor(1.329924, 103.715837), GPSCoor(1.4431, 103.664992), GPSCoor(1.375685, 103.681508),
 GPSCoor(1.365208, 103.686296), GPSCoor(1.424941, 103.672176), GPSCoor(1.360802, 103.775598), GPSCoor(1.335877, 103.787019),
 GPSCoor(1.30538, 104.01478), GPSCoor(1.2982, 103.76638), GPSCoor(1.296208, 104.017724), GPSCoor(1.318032, 104.019381),
 GPSCoor(1.29872, 103.614428), GPSCoor(1.33947, 103.818956), GPSCoor(1.459945, 103.8009), GPSCoor(1.44594, 103.625052),
 GPSCoor(1.244757, 103.766312), GPSCoor(1.324431, 103.86616), GPSCoor(1.396491, 103.868897), GPSCoor(1.356078, 103.7154),
 GPSCoor(1.26365, 103.716774), GPSCoor(1.240679, 103.941206), GPSCoor(1.369473, 103.847963), GPSCoor(1.357588, 103.748736),
 GPSCoor(1.415317, 103.771497), GPSCoor(1.391571, 103.900681), GPSCoor(1.373651, 103.903547), GPSCoor(1.368656, 103.890962),
 GPSCoor(1.374568, 103.76065), GPSCoor(1.417024, 103.850344), GPSCoor(1.257238, 103.977422), GPSCoor(1.291157, 104.013593),
 GPSCoor(1.428272, 103.912225), GPSCoor(1.452585, 103.741225), GPSCoor(1.351012, 103.622477), GPSCoor(1.424163, 103.912549),
 GPSCoor(1.282951, 103.710222), GPSCoor(1.290813, 103.948552), GPSCoor(1.468102, 103.989638), GPSCoor(1.392675, 103.857576),
 GPSCoor(1.463479, 104.015896), GPSCoor(1.314384, 104.017671), GPSCoor(1.399155, 103.982785), GPSCoor(1.458108, 103.865159),
 GPSCoor(1.380629, 103.811487), GPSCoor(1.313076, 103.975868), GPSCoor(1.390425, 103.914551), GPSCoor(1.242484, 103.758237),
 GPSCoor(1.441834, 103.955128), GPSCoor(1.405489, 103.768983), GPSCoor(1.294853, 103.804878), GPSCoor(1.267392, 103.800269),
 GPSCoor(1.323297, 103.641291), GPSCoor(1.464512, 103.73284), GPSCoor(1.310067, 103.94299), GPSCoor(1.437196, 103.709033),
 GPSCoor(1.457183, 103.996254), GPSCoor(1.448901, 104.017396), GPSCoor(1.311454, 103.640521), GPSCoor(1.285862, 103.993216),
 GPSCoor(1.453862, 103.973669), GPSCoor(1.366741, 103.955035), GPSCoor(1.451393, 103.693541), GPSCoor(1.25601, 103.894478),
 GPSCoor(1.31399, 103.957373), GPSCoor(1.40948, 103.745138), GPSCoor(1.37108, 103.789396), GPSCoor(1.373036, 103.652644),
 GPSCoor(1.462632, 103.930858), GPSCoor(1.429354, 103.763949), GPSCoor(1.45889, 103.936351), GPSCoor(1.308417, 103.879371),
 GPSCoor(1.384351, 103.676438), GPSCoor(1.40288, 103.808186), GPSCoor(1.466354, 104.009928), GPSCoor(1.393119, 103.797958),
 GPSCoor(1.267539, 103.932432), GPSCoor(1.281722, 103.905296), GPSCoor(1.357859, 103.745774), GPSCoor(1.389625, 103.914025),
 GPSCoor(1.399105, 103.675278), GPSCoor(1.432151, 103.618066), GPSCoor(1.426014, 103.683805), GPSCoor(1.243116, 103.976549),
 GPSCoor(1.350854, 103.908361), GPSCoor(1.247891, 103.928943), GPSCoor(1.242248, 103.930405), GPSCoor(1.405005, 103.618868),
 GPSCoor(1.297484, 103.889279), GPSCoor(1.344891, 103.820866), GPSCoor(1.281964, 104.024863), GPSCoor(1.388096, 103.662146),
 GPSCoor(1.439224, 104.0031), GPSCoor(1.272519, 103.959806), GPSCoor(1.398643, 103.946107), GPSCoor(1.368977, 103.848406),
 GPSCoor(1.383106, 103.773928), GPSCoor(1.292903, 103.711608), GPSCoor(1.357511, 103.778891), GPSCoor(1.267991, 103.764238),
 GPSCoor(1.331303, 103.843202), GPSCoor(1.443582, 103.957358), GPSCoor(1.447129, 103.880124), GPSCoor(1.334045, 103.716822),
 GPSCoor(1.380874, 103.917424), GPSCoor(1.385655, 103.932797), GPSCoor(1.261726, 103.708728), GPSCoor(1.372147, 103.709716),
 GPSCoor(1.388583, 103.894022), GPSCoor(1.266672, 103.63567), GPSCoor(1.341483, 103.943733), GPSCoor(1.279299, 103.961935),
 GPSCoor(1.389771, 103.93696), GPSCoor(1.446799, 103.919161), GPSCoor(1.300831, 103.624476), GPSCoor(1.270087, 103.761307), 
 GPSCoor(1.24965, 103.956689), GPSCoor(1.316122, 103.914245), GPSCoor(1.283248, 103.665979), GPSCoor(1.267085, 103.749931),
 GPSCoor(1.341417, 103.891425), GPSCoor(1.307928, 104.025196), GPSCoor(1.271215, 103.773519), GPSCoor(1.332214, 103.944932), 
 GPSCoor(1.420984, 103.878368), GPSCoor(1.462042, 103.825669), GPSCoor(1.457267, 103.804095), GPSCoor(1.338469, 104.001467), 
 GPSCoor(1.264154, 103.797257), GPSCoor(1.322406, 103.930585), GPSCoor(1.421604, 103.966424), GPSCoor(1.39603, 103.69299),
 GPSCoor(1.349401, 103.634814), GPSCoor(1.30386, 103.673344), GPSCoor(1.242775, 103.826114), GPSCoor(1.278221, 103.95559),
 GPSCoor(1.351691, 103.774884), GPSCoor(1.331565, 103.77779), GPSCoor(1.44321, 103.944907), GPSCoor(1.413164, 103.90539),
 GPSCoor(1.390478, 104.019575), GPSCoor(1.386821, 103.976866), GPSCoor(1.273295, 103.853298), GPSCoor(1.446714, 103.830314),
 GPSCoor(1.429048, 103.952054), GPSCoor(1.392735, 103.784015), GPSCoor(1.371979, 103.667861), GPSCoor(1.384016, 103.814927),
 GPSCoor(1.353758, 103.92828), GPSCoor(1.29709, 103.941074), GPSCoor(1.302803, 103.819106), GPSCoor(1.309877, 103.657405),
 GPSCoor(1.452288, 103.970861), GPSCoor(1.319042, 103.986369), GPSCoor(1.351962, 103.864484), GPSCoor(1.446105, 103.648827),
 GPSCoor(1.259793, 103.83018), GPSCoor(1.438446, 103.704908), GPSCoor(1.342185, 103.760643), GPSCoor(1.437552, 103.720919),
 GPSCoor(1.244621, 103.940131), GPSCoor(1.328186, 103.70131), GPSCoor(1.294209, 103.739065), GPSCoor(1.316017, 103.62242),
 GPSCoor(1.313023, 103.702464), GPSCoor(1.4233, 103.67168), GPSCoor(1.268474, 103.844053), GPSCoor(1.29127, 103.966833),
 GPSCoor(1.31288, 104.024904), GPSCoor(1.25471, 103.706942), GPSCoor(1.281805, 103.626781), GPSCoor(1.347056, 103.995045),
 GPSCoor(1.323146, 103.910886), GPSCoor(1.405895, 103.698105), GPSCoor(1.264882, 103.929677), GPSCoor(1.286067, 103.950441),
 GPSCoor(1.444767, 104.023424), GPSCoor(1.312254, 103.708185), GPSCoor(1.434661, 103.98316), GPSCoor(1.280745, 103.624372),
 GPSCoor(1.467727, 103.94081), GPSCoor(1.389865, 103.913426), GPSCoor(1.290784, 103.648681), GPSCoor(1.241475, 103.772717),
 GPSCoor(1.422101, 103.774841), GPSCoor(1.395199, 103.82029), GPSCoor(1.264415, 103.687989), GPSCoor(1.261705, 104.02164),
 GPSCoor(1.393167, 103.642034), GPSCoor(1.419901, 103.686953), GPSCoor(1.438801, 103.689743), GPSCoor(1.294682, 103.957383),
 GPSCoor(1.328751, 103.740737), GPSCoor(1.267114, 103.653197), GPSCoor(1.331208, 103.803175), GPSCoor(1.431102, 103.889584),
 GPSCoor(1.420153, 103.833116), GPSCoor(1.463896, 103.868344), GPSCoor(1.246673, 103.957588), GPSCoor(1.423707, 103.822991),
 GPSCoor(1.458246, 103.922526), GPSCoor(1.244947, 103.66307), GPSCoor(1.26848, 103.980408), GPSCoor(1.291343, 104.000434), 
 GPSCoor(1.278358, 103.834688), GPSCoor(1.333523, 103.765469), GPSCoor(1.455806, 103.847421), GPSCoor(1.330956, 103.643562), 
 GPSCoor(1.254674, 103.740212), GPSCoor(1.365565, 103.87815), GPSCoor(1.461472, 103.900485), GPSCoor(1.438816, 103.960011),
 GPSCoor(1.450021, 103.987975), GPSCoor(1.287926, 103.74452), GPSCoor(1.312318, 103.78192), GPSCoor(1.303709, 103.849035),
 GPSCoor(1.445379, 103.800952), GPSCoor(1.360232, 103.976364), GPSCoor(1.265435, 103.695231), GPSCoor(1.410545, 103.690397),
 GPSCoor(1.31536, 103.952753), GPSCoor(1.40099, 103.692384), GPSCoor(1.416447, 103.70867), GPSCoor(1.389356, 103.775602),
 GPSCoor(1.387757, 103.891925), GPSCoor(1.256423, 103.944568), GPSCoor(1.311247, 103.958238), GPSCoor(1.383236, 103.838309), 
 GPSCoor(1.349812, 103.668351), GPSCoor(1.359233, 103.642268), GPSCoor(1.362467, 103.712447), GPSCoor(1.423655, 103.955873),
 GPSCoor(1.354948, 103.691811), GPSCoor(1.311758, 103.733155), GPSCoor(1.348371, 103.715918), GPSCoor(1.328196, 103.864557),
 GPSCoor(1.430555, 103.719885), GPSCoor(1.32807, 103.978956), GPSCoor(1.257179, 103.851874), GPSCoor(1.292409, 103.990876),
 GPSCoor(1.316146, 103.866051), GPSCoor(1.277989, 103.618046), GPSCoor(1.407886, 103.751844), GPSCoor(1.453226, 103.61969),
 GPSCoor(1.377936, 103.995352), GPSCoor(1.312702, 103.717795), GPSCoor(1.439274, 103.981209), GPSCoor(1.272219, 103.894458),
 GPSCoor(1.296243, 103.969185), GPSCoor(1.26088, 103.808582), GPSCoor(1.367554, 103.680276), GPSCoor(1.275911, 103.679045),
 GPSCoor(1.434154, 103.725466), GPSCoor(1.270054, 103.639966), GPSCoor(1.294958, 103.748433), GPSCoor(1.390142, 103.998808), 
 GPSCoor(1.371662, 103.720269), GPSCoor(1.341712, 103.669836), GPSCoor(1.409258, 103.741567), GPSCoor(1.459805, 103.949213),
 GPSCoor(1.379025, 103.633888), GPSCoor(1.365296, 103.933956), GPSCoor(1.459493, 103.908991), GPSCoor(1.270991, 103.934913),
 GPSCoor(1.278341, 104.021782), GPSCoor(1.243058, 103.924632), GPSCoor(1.271218, 103.730577), GPSCoor(1.369294, 103.967906),
 GPSCoor(1.271683, 103.683602), GPSCoor(1.36591, 103.696459), GPSCoor(1.354206, 103.846409), GPSCoor(1.323211, 103.942137),
 GPSCoor(1.315831, 103.659846), GPSCoor(1.390925, 104.015792), GPSCoor(1.270673, 103.83262), GPSCoor(1.368146, 103.831595),
 GPSCoor(1.33608, 103.832008), GPSCoor(1.421429, 103.65406), GPSCoor(1.285251, 103.884078), GPSCoor(1.26686, 103.996664),
 GPSCoor(1.39563, 103.752675), GPSCoor(1.37855, 103.617926), GPSCoor(1.322178, 103.739318), GPSCoor(1.413102, 103.800122),
 GPSCoor(1.272053, 103.99393), GPSCoor(1.426827, 103.768623), GPSCoor(1.399271, 103.681592), GPSCoor(1.466912, 103.907096), 
 GPSCoor(1.367552, 103.63416), GPSCoor(1.3586, 103.820685), GPSCoor(1.282833, 103.688446), GPSCoor(1.349969, 103.834349),
 GPSCoor(1.342339, 103.838435), GPSCoor(1.24934, 103.683662), GPSCoor(1.441395, 103.916611), GPSCoor(1.465202, 103.89536),
 GPSCoor(1.305607, 104.002183), GPSCoor(1.419223, 103.955178), GPSCoor(1.422598, 103.741582), GPSCoor(1.298965, 103.664358),
 GPSCoor(1.267366, 103.947915), GPSCoor(1.289723, 103.936843), GPSCoor(1.34542, 103.675948), GPSCoor(1.288296, 103.719038),
 GPSCoor(1.434253, 104.029522), GPSCoor(1.324346, 103.671298), GPSCoor(1.402958, 103.61184), GPSCoor(1.27586, 103.715456),
 GPSCoor(1.408118, 103.975344), GPSCoor(1.320056, 103.702472), GPSCoor(1.364124, 103.677889), GPSCoor(1.332867, 103.751641),
 GPSCoor(1.29308, 103.754579), GPSCoor(1.284949, 103.872359), GPSCoor(1.322348, 103.780184), GPSCoor(1.448495, 104.004118),
 GPSCoor(1.359028, 103.899846), GPSCoor(1.274704, 103.891213), GPSCoor(1.300878, 103.954269), GPSCoor(1.426172, 103.625071),
 GPSCoor(1.357336, 103.632501), GPSCoor(1.24852, 103.643622), GPSCoor(1.387502, 103.696951), GPSCoor(1.329218, 103.821845),
 GPSCoor(1.240697, 103.683285), GPSCoor(1.244001, 104.000701), GPSCoor(1.250845, 103.694217), GPSCoor(1.390431, 104.018201),
 GPSCoor(1.31754, 103.970089), GPSCoor(1.406794, 103.725499), GPSCoor(1.296142, 103.666218), GPSCoor(1.375435, 103.848048),
 GPSCoor(1.443001, 103.846046), GPSCoor(1.399498, 103.674664), GPSCoor(1.344083, 103.748784), GPSCoor(1.255347, 103.955097),
 GPSCoor(1.357486, 103.727618), GPSCoor(1.335857, 103.84372), GPSCoor(1.467464, 103.998243), GPSCoor(1.371026, 103.764417),
 GPSCoor(1.261294, 103.656077), GPSCoor(1.400639, 103.911836), GPSCoor(1.269037, 103.719157), GPSCoor(1.291898, 103.869338),
 GPSCoor(1.345761, 103.65813), GPSCoor(1.315398, 103.686839), GPSCoor(1.40812, 103.696873), GPSCoor(1.441529, 104.02082),
 GPSCoor(1.462515, 103.631693), GPSCoor(1.422912, 104.006205), GPSCoor(1.24144, 103.732627), GPSCoor(1.400035, 103.693618),
 GPSCoor(1.348076, 103.677993), GPSCoor(1.361434, 103.807269), GPSCoor(1.389498, 103.80587), GPSCoor(1.381544, 103.616569),
 GPSCoor(1.417569, 103.660136), GPSCoor(1.408566, 103.617902), GPSCoor(1.4157, 103.649754), GPSCoor(1.305628, 103.975876),
 GPSCoor(1.358318, 103.634833), GPSCoor(1.382744, 103.831693), GPSCoor(1.25925, 103.704246), GPSCoor(1.464791, 103.908475),
 GPSCoor(1.252786, 103.924946), GPSCoor(1.280741, 103.900322), GPSCoor(1.258491, 103.919619), GPSCoor(1.439692, 103.865651),
 GPSCoor(1.284316, 103.765047), GPSCoor(1.462014, 103.936074), GPSCoor(1.333369, 103.799796), GPSCoor(1.403225, 103.679942),
 GPSCoor(1.434695, 103.647975), GPSCoor(1.345831, 103.803916), GPSCoor(1.34182, 103.755848), GPSCoor(1.32333, 103.757751),
 GPSCoor(1.448897, 103.988441), GPSCoor(1.438742, 103.726172), GPSCoor(1.377176, 103.969528), GPSCoor(1.370089, 103.767669),
 GPSCoor(1.427384, 103.829366), GPSCoor(1.245369, 103.877851), GPSCoor(1.392425, 103.62316), GPSCoor(1.243184, 103.690114),
 GPSCoor(1.317882, 103.68132), GPSCoor(1.451465, 103.972642), GPSCoor(1.328465, 103.898655), GPSCoor(1.307341, 103.655459),
 GPSCoor(1.430296, 103.85223), GPSCoor(1.417543, 103.828183), GPSCoor(1.454821, 103.783988), GPSCoor(1.409689, 103.681785),
 GPSCoor(1.463541, 103.908679), GPSCoor(1.294268, 103.625394), GPSCoor(1.39106, 103.770355), GPSCoor(1.359022, 103.944167),
 GPSCoor(1.357573, 103.888687), GPSCoor(1.411768, 103.732361), GPSCoor(1.292693, 104.015985), GPSCoor(1.323556, 103.892145),
 GPSCoor(1.263738, 103.934564), GPSCoor(1.423603, 103.865278), GPSCoor(1.367141, 103.88432), GPSCoor(1.439976, 103.983044), 
 GPSCoor(1.245424, 103.632986), GPSCoor(1.361842, 103.710543), GPSCoor(1.413858, 103.67059), GPSCoor(1.394257, 103.718074),
 GPSCoor(1.441168, 103.660457), GPSCoor(1.467326, 103.64909), GPSCoor(1.28332, 103.755014), GPSCoor(1.424053, 103.751336),
 GPSCoor(1.254918, 103.863022), GPSCoor(1.278969, 103.950163), GPSCoor(1.374799, 104.004665), GPSCoor(1.259375, 103.708941),
 GPSCoor(1.404897, 103.667895), GPSCoor(1.259369, 103.628051), GPSCoor(1.351612, 103.831336), GPSCoor(1.459478, 103.875894),
 GPSCoor(1.467948, 103.744228), GPSCoor(1.317372, 103.856482), GPSCoor(1.340374, 103.876662), GPSCoor(1.287941, 103.806996),
 GPSCoor(1.35405, 103.967221), GPSCoor(1.279724, 103.701136), GPSCoor(1.423834, 103.992292), GPSCoor(1.268923, 103.795286),
 GPSCoor(1.319869, 103.643784), GPSCoor(1.455834, 103.770454), GPSCoor(1.365012, 103.725479), GPSCoor(1.313913, 103.796066),
 GPSCoor(1.339045, 103.794757), GPSCoor(1.38233, 103.994208), GPSCoor(1.244477, 103.679302), GPSCoor(1.402494, 103.705252),
 GPSCoor(1.337758, 103.759467), GPSCoor(1.307307, 104.023517), GPSCoor(1.377843, 103.994218), GPSCoor(1.457012, 103.83656),
 GPSCoor(1.337205, 103.923675), GPSCoor(1.33005, 103.931141), GPSCoor(1.466371, 103.755708), GPSCoor(1.245888, 103.642207),
 GPSCoor(1.376943, 103.765974), GPSCoor(1.32701, 103.843758), GPSCoor(1.380666, 103.899394), GPSCoor(1.249463, 103.939396),
 GPSCoor(1.362332, 103.713641), GPSCoor(1.408896, 103.773244), GPSCoor(1.365146, 103.700291), GPSCoor(1.278054, 103.884399),
 GPSCoor(1.451000, 103.820772), GPSCoor(1.451305, 103.819006), GPSCoor(1.449268, 103.819797), GPSCoor(1.18560, 103.5188)
};                                              
// what's the name of the hardware serial port?
#define GPSSerial Serial1
// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false
uint32_t timer = millis();
#define DEBUG_GPS false
#define DEBUG_OUT false

boolean is_bus_stop = false;
float gpslat = 0.0000f;
float gpslon = 0.0000f;
LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
#define TX_POWER_LEVEL 23
// power and state indicator
#define LED_POWER 13
#define LED_INDICATOR 12
#define GPS_STATUS_ADDRESS 8 // connects to GPS; 0 - NO GPGS / in transit ; 1 - GPS found; bus stop near by
#define WAIT_TIME 5000 // 20 seconds
#define STATE_IDLE 0xA0
#define STATE_REQUEST_SIGNAL_VALID 0xA1
#define STATE_TIMER_COUNT 0xA2
#define STATE_GPS_VALID 0xA3

unsigned long currentState = STATE_IDLE;
char* requestRxToken;
const char RESPONSE_TOKEN[20] = "1234567890123456789";
const char REQUEST_TOKEN[20] = "ABCDEFGH1JKLMNOPQRS";
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
elapsedMillis waitingTimeElapsed;
uint8_t c = 0;

char cursor_busy[5] = {'-', '|', '+', 'x', '*'};
int ci = 0;

void clearlcd(){
  lcd.init();
  lcd.backlight();
}
void text(char txt[], uint8_t col = 0, uint8_t row = 0, boolean doclear = false){
  if (doclear){
    clearlcd();
  }
  lcd.setCursor(col,row);
  lcd.print(txt);
}
void out(String v){
  if (DEBUG_OUT)
    Serial.println(v);
}
void blinkState(){
  uint8_t ledState = digitalRead(LED_INDICATOR);  // read input value
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(LED_INDICATOR, ledState);
}
void countlcd(){
  lcd.setCursor(9,2);
  lcd.print(++c);   
}
void cursorDisplay(){
  if (ci >= 5) ci = 0;
  lcd.setCursor(19,2);
  lcd.print(String(cursor_busy[ci++]));
}
void lora(){
  out("check lora signal");
  while(currentState == STATE_IDLE){
    requestEvent();
    // out("idle state");
    if (rf95.available()){
      // Should be a message for us now   
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      if (rf95.recv(buf, &len)) {
        RH_RF95::printBuffer("Received: ", buf, len);
        Serial.print("Got: ");
        requestRxToken = (char*)buf;
        out(requestRxToken);
        // compare token if valid
        // check if GPS is valid        
        if (is_bus_stop){
            if (strcmp(requestRxToken, REQUEST_TOKEN) == 0){
              Serial.print("RSSI: ");
              Serial.print(rf95.lastRssi(), DEC);
              delay(10);
              // Send a reply
              delay(200); // may or may not be needed
              // replay validation message
              uint8_t data[] = "1234567890123456789";
              rf95.send(data, sizeof(data));
              rf95.waitPacketSent();
              out("Sent a reply");
              out(">> VALIDATION SENT BACK");
              text("> Signal found <",2,0,true);              
              text("Boarding...",5,1);
              currentState = STATE_REQUEST_SIGNAL_VALID;waitingTimeElapsed = 0; c = 0;
            } else {
              out("invalid token");
            }
          }else{
            out("GPS invalid");
          }
        } else {
          out("Receive failed");
        }
    }
  }
  while(currentState == STATE_REQUEST_SIGNAL_VALID){
    // blink LED blink here; start time elapse count
    // delay accounts for doors and bus proceeding to next stop; ignores other signals for 5 secs
    blinkState();delay(500);blinkState();delay(500);countlcd();
    if (waitingTimeElapsed > WAIT_TIME){
      digitalWrite(LED_INDICATOR, HIGH);
      text("Doors are closing",1,0, true);
      text(">                  <",0,1);delay(2000);
      text(">>>              <<<",0,1);delay(500);
      text(">>>>>          <<<<<",0,1);delay(500);
      text(">>>>>>>>    <<<<<<<<",0,1);delay(500);
      text(">>>>>>>>>><<<<<<<<<<",0,1);delay(500);
      delay(5000);
      // waiting time; reset to idle state; reset the counter to 0 so the counting starts over...
      waitingTimeElapsed = 0; c = 0; currentState = STATE_IDLE;
      text("*** Bus Stopper ***",0,0, true);
      text(">Waiting for signal<",0,1);
      text("Bus stop:", 0, 2, false);
      is_bus_stop = false;
      digitalWrite(LED_INDICATOR, LOW);      
    }
  }
}
void requestEvent() {
  // Serial.println("request event...");
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) {// this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
    }
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
  // approximately every 2 seconds or so, print out the current stats  
  if (millis() - timer > 2000) {
    cursorDisplay();
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    
    if (DEBUG_GPS){
      // debug to test w/o GPS
      // gpslat = 1.55551926;
      // gpslon = 101.0763351;
      gpslat = 1.33333;
      gpslon = 103.55555;
    }
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      gpslat = GPS.latitude / 100;
      gpslon = GPS.longitude / 100;
    }
    char result[18] = {};
    sprintf(result, "%.5f,%.5f", gpslat, gpslon);
    is_bus_stop = is_gps_coordinates_near(GPSCoor(gpslat, gpslon));
    text(result, 0, 3, false);
    
    if (!GPS.fix){
      text("No Data", 9, 2, false);
    }else{
      if (is_bus_stop){
        text("Yes       ", 9, 2, false);
      }else{
        text("In-transit", 9, 2, false);
      }      
    }
    delay(1000);
    Serial.println("request");
  }
  delay(100);
}

// checks if gps distance with tolerable distance range from the list of bus stations
boolean is_gps_coordinates_near(GPSCoor coor){
  Serial.println("compute distance ");
  if (DEBUG_GPS){
    Serial.println("debug gps return true");
    return true;
  }
  if (coor.lon() == 0 && coor.lat() == 0){
    Serial.println("invalid gps");
    return false;
  }
  for(int i = 0; i < NUMBER_OF_STATIONS; ++i){    
    float ds = gpsdistance(coor, busstops[i]);
    if ((ds >= 0) && (ds <= GPS_STATION_DISTANCE_RANGE)) {
      return true;
    }
  }
  return false;
}
float gpsdistance(GPSCoor o1, GPSCoor o2){
  float d = haversine_km(o1.lat(), o1.lon(), o2.lat(), o2.lon());
  if (DEBUG_GPS){
    Serial.print("checking...");
    Serial.print(o1.coordinates());Serial.print(" - ");
    Serial.println(o2.coordinates());
    Serial.print("distance: ");Serial.println(d);
  }
  return d;
}
// calculate haversine distance in kilometers for linear distance; coordinates in degress
float haversine_km(float lat1, float long1, float lat2, float long2){
    float dlong = (long2 - long1) * d2r;
    float dlat = (lat2 - lat1) * d2r;
    float a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    float d = 6367 * c;
    return d;
}
void setup()
{
  // === LoRA
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_INDICATOR, OUTPUT);
  digitalWrite(LED_POWER, HIGH);
  pinMode(RFM95_RST, OUTPUT);  
  //setup i2c for GPS; request the data from GPS
  // while (!Serial); uncomment when debugging
  Serial.begin(115200);
  delay(100);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    out("LoRa radio init failed");
    while (1);
  }
  out("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    out("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(TX_POWER_LEVEL, false);
  text("*** Bus Stopper ***",0,0, true);
  text(">Waiting for signal<",0,1);
  
  // === GPS
  //while (!Serial); // uncomment to have the sketch wait until Serial is ready; uncomment if not debugging or in power pack USB 
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  //Serial.begin(115200);
  Serial.println("Adafruit GPS library...");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  delay(100);
  text("Bus stop:", 0, 2, false);
}
void loop(){
  delay(5000);  
  lora();
}

