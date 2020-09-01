EULER = euler_1 euler_2 euler_3 euler_4 euler_5 euler_6 euler_7 euler_8 \
        euler_9 euler_10 euler_11 euler_12 euler_13 euler_14 euler_15 euler_16 \
        euler_17 euler_18 euler_19 euler_20 euler_21 euler_22 euler_23 \
        euler_24 euler_25 euler_26 euler_27 euler_28 euler_29 euler_30 \
        euler_31 euler_32 euler_33 euler_34 euler_35 euler_36 euler_37 \
        euler_38 euler_39 euler_40 euler_41 euler_42 euler_43 euler_44 \
        euler_45 euler_46 euler_47 euler_48 euler_49 euler_50 euler_51 \
        euler_52 euler_53 euler_54 euler_55 euler_56 euler_57 euler_58 \
        euler_59 euler_60 euler_61 euler_62 euler_63 euler_64 euler_65 \
        euler_66 euler_67 euler_68 euler_69 euler_70 euler_71 euler_72 \
        euler_73 euler_74 euler_75 euler_76 euler_77 euler_78 euler_79 \
        euler_80 euler_81 euler_82 euler_83 euler_84 euler_85 euler_86 \
        euler_87 euler_88 euler_89 euler_90 euler_91 euler_92 euler_93 \
        euler_94 euler_95 euler_96 euler_97 euler_98 euler_99 euler_100 \
        euler_101 euler_102 euler_145 euler_206

CXX = clang++80

SRCS = ${EULER:=.cpp}
OBJS = ${SRCS:.cpp=.o}

all: ${OBJS}

clean:
	rm -rf *~ *.o ${EULER}

SUFFIXES: .cpp .o

.cpp:
	bear -a ${CXX} ${CXXFLAGS} ${.IMPSRC} -o ${.TARGET}

.cpp.o:
	bear -a ${CXX} ${CXXFLAGS} -c ${.IMPSRC}
