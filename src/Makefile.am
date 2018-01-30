AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=pcafacerecog
pcafacerecog_SOURCES= face_recognition.cpp My_Matrix.cpp Process.cpp 
pcafacerecog_CXXFLAGS=$(opencv_CFLAGS) $(glib2_CFLAGS) -Wall -Wno-misleading-indentation
pcafacerecog_LDADD=$(opencv_LIBS) $(glib2_LIBS)

