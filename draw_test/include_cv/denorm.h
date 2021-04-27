/// @file denorm.h
/// @brief copy from tesseract-OCR (https://github.com/tesseract-ocr/tesseract) 

#pragma once

#define _USE_MATH_DEFINES	// for M_PI
#define GRAPHICS_DISABLED

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <cmath>


/*Control parameters for error()*/
enum TessErrorLogCode {
	DBG = -1,           /*log without alert */
	TESSLOG = 0,            /*alert user */
	TESSEXIT = 1,            /*exit after erro */
	ABORT = 2            /*abort after error */
};

constexpr ERRCODE BADERRACTION("Illegal error action");
#define MAX_MSG       1024

class ERRCODE {           // error handler class
	const char *message;           // error message
public:
	void error(             // handle error
		const char *caller,              // name of caller
		TessErrorLogCode action,         // action to take
		const char *format, ...          // special message
	) const {
		va_list args;                  // variable args
		char msg[MAX_MSG];
		char *msgptr = msg;

		if (caller != nullptr)
			//name of caller
			msgptr += sprintf(msgptr, "%s:", caller);
		//actual message
		msgptr += sprintf(msgptr, "Error:%s", message);
		if (format != nullptr) {
			msgptr += sprintf(msgptr, ":");
			va_start(args, format);  //variable list
#ifdef _WIN32
							 //print remainder
			msgptr += _vsnprintf(msgptr, MAX_MSG - 2 - (msgptr - msg), format, args);
			msg[MAX_MSG - 2] = '\0';     //ensure termination
			strcat(msg, "\n");
#else
							 //print remainder
			msgptr += vsprintf(msgptr, format, args);
			//no specific
			msgptr += sprintf(msgptr, "\n");
#endif
			va_end(args);
		}
		else
			//no specific
			msgptr += sprintf(msgptr, "\n");

		// %s is needed here so msg is printed correctly!
		fprintf(stderr, "%s", msg);

		switch (action) {
		case DBG:
		case TESSLOG:
			return;                    //report only
		case TESSEXIT:
		case ABORT:
#if !defined(NDEBUG)
			// Create a deliberate abnormal exit as the stack trace is more useful
			// that way. This is done only in debug builds, because the
			// error message "segmentation fault" confuses most normal users.
#if defined(__GNUC__)
			__builtin_trap();
#else
			*reinterpret_cast<int*>(0) = 0;
#endif
#endif
			abort();
		default:
			BADERRACTION.error("error", ABORT, nullptr);
		}
	}

	constexpr ERRCODE(const char *string) : message(string) {
	}                            // initialize with string
};

constexpr ERRCODE ASSERT_FAILED("Assert failed");

#if defined __cplusplus
# define DO_NOTHING static_cast<void>(0)
#else
# define DO_NOTHING (void)(0)
#endif

#define ASSERT_HOST(x) (x) \
  ? DO_NOTHING \
  : ASSERT_FAILED.error(#x, ABORT, "in file %s, line %d", __FILE__, __LINE__)

#define ASSERT_HOST_MSG(x, ...)                                                \
  if (!(x)) {                                                                  \
    tprintf(__VA_ARGS__);                                                      \
    ASSERT_FAILED.error(#x, ABORT, "in file %s, line %d", __FILE__, __LINE__); \
  }

// The fortran/basic sgn function returns -1, 0, 1 if x < 0, x == 0, x > 0
// respectively.
static int sign(int x) {
	if (x < 0)
		return -1;
	else
		return x > 0 ? 1 : 0;
}


//------------------------------
// serialize  / deserialize
namespace tesseract {

	static
		bool DeSerialize(FILE* fp, char* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, float* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, int8_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, int16_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, int32_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, uint8_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, uint16_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool DeSerialize(FILE* fp, uint32_t* data, size_t n = 1) {
		return fread(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const char* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const float* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const int8_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const int16_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const int32_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const uint8_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const uint16_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
	static
		bool Serialize(FILE* fp, const uint32_t* data, size_t n = 1) {
		return fwrite(data, sizeof(*data), n, fp) == n;
	}
}
//------------------------------

//------------------------------
// tprintf
//#define MAX_MSG_LEN 2048
//
//#define STRING_VAR(name, val, comment) \
//  std::string name(val)
//
//static STRING_VAR(debug_file, "", "File to send tprintf output to");
//
//std::mutex tprintfMutex;
//
//static void tprintf(const char *format, ...)
//{
//	//tesseract::tprintfMutex.Lock();
//	std::lock_guard<std::mutex> lg(tprintfMutex);
//
//	va_list args;                  // variable args
//	const char* debug_file_name = debug_file.c_str();
//	static FILE *debugfp = nullptr;   // debug file
//								   // debug window
//	int32_t offset = 0;              // into message
//	char msg[MAX_MSG_LEN + 1];
//
//	va_start(args, format);  // variable list
//	// Format into msg
//#ifdef _WIN32
//	offset += _vsnprintf(msg + offset, MAX_MSG_LEN - offset, format, args);
//	if (debug_file_name && strcmp(debug_file_name, "/dev/null") == 0)
//		debug_file.set_value("nul");
//#else
//	offset += vsnprintf(msg + offset, MAX_MSG_LEN - offset, format, args);
//#endif
//	va_end(args);
//
//	if (debugfp == nullptr && debug_file_name && strlen(debug_file_name) > 0) {
//		debugfp = fopen(debug_file.string(), "wb");
//	}
//	else if (debugfp != nullptr && debug_file_name && strlen(debug_file_name) == 0) {
//		fclose(debugfp);
//		debugfp = nullptr;
//	}
//	if (debugfp != nullptr)
//		fprintf(debugfp, "%s", msg);
//	else
//		fprintf(stderr, "%s", msg);
//	
//	//tesseract::tprintfMutex.Unlock();
//}
static void tprintf(const char *format, ...) {}
//------------------------------


//------------------------------
// helpers

// Remove newline (if any) at the end of the string.
inline void chomp_string(char* str) {
  int last_index = static_cast<int>(strlen(str)) - 1;
  while (last_index >= 0 &&
         (str[last_index] == '\n' || str[last_index] == '\r')) {
    str[last_index--] = '\0';
  }
}

// Advance the current pointer of the file if it points to a newline character.
inline void SkipNewline(FILE* file) {
  if (fgetc(file) != '\n') {
    fseek(file, -1, SEEK_CUR);
  }
}

// Swaps the two args pointed to by the pointers.
// Operator= and copy constructor must work on T.
template <typename T>
inline void Swap(T* p1, T* p2) {
  T tmp(*p2);
  *p2 = *p1;
  *p1 = tmp;
}

// return the smallest multiple of block_size greater than or equal to n.
inline int RoundUp(int n, int block_size) {
  return block_size * ((n + block_size - 1) / block_size);
}

// Clip a numeric value to the interval [lower_bound, upper_bound].
template <typename T>
inline T ClipToRange(const T& x, const T& lower_bound, const T& upper_bound) {
  if (x < lower_bound) {
    return lower_bound;
  }
  if (x > upper_bound) {
    return upper_bound;
  }
  return x;
}

// Extend the range [lower_bound, upper_bound] to include x.
template <typename T1, typename T2>
inline void UpdateRange(const T1& x, T2* lower_bound, T2* upper_bound) {
  if (x < *lower_bound) {
    *lower_bound = x;
  }
  if (x > *upper_bound) {
    *upper_bound = x;
  }
}

// Decrease lower_bound to be <= x_lo AND increase upper_bound to be >= x_hi.
template <typename T1, typename T2>
inline void UpdateRange(const T1& x_lo, const T1& x_hi, T2* lower_bound,
                        T2* upper_bound) {
  if (x_lo < *lower_bound) {
    *lower_bound = x_lo;
  }
  if (x_hi > *upper_bound) {
    *upper_bound = x_hi;
  }
}

// Intersect the range [*lower2, *upper2] with the range [lower1, upper1],
// putting the result back in [*lower2, *upper2].
// If non-intersecting ranges are given, we end up with *lower2 > *upper2.
template <typename T>
inline void IntersectRange(const T& lower1, const T& upper1, T* lower2,
                           T* upper2) {
  if (lower1 > *lower2) {
    *lower2 = lower1;
  }
  if (upper1 < *upper2) {
    *upper2 = upper1;
  }
}

// Proper modulo arithmetic operator. Returns a mod b that works for -ve a.
// For any integer a and positive b, returns r : 0<=r<b and a=n*b + r for
// some integer n.
inline int Modulo(int a, int b) {
  return (a % b + b) % b;
}

// Integer division operator with rounding that works for negative input.
// Returns a divided by b, rounded to the nearest integer, without double
// counting at 0. With simple rounding 1/3 = 0, 0/3 = 0 -1/3 = 0, -2/3 = 0,
// -3/3 = 0 and -4/3 = -1.
// I want 1/3 = 0, 0/3 = 0, -1/3 = 0, -2/3 = -1, -3/3 = -1 and -4/3 = -1.
inline int DivRounded(int a, int b) {
  if (b < 0) {
    return -DivRounded(a, -b);
  }
  return a >= 0 ? (a + b / 2) / b : (a - b / 2) / b;
}

// Return a double cast to int with rounding.
inline int IntCastRounded(double x) {
  return x >= 0.0 ? static_cast<int>(x + 0.5) : -static_cast<int>(-x + 0.5);
}

// Return a float cast to int with rounding.
inline int IntCastRounded(float x) {
  return x >= 0.0F ? static_cast<int>(x + 0.5F) : -static_cast<int>(-x + 0.5F);
}

// Reverse the order of bytes in a n byte quantity for big/little-endian switch.
inline void ReverseN(void* ptr, int num_bytes) {
  assert(num_bytes == 1 || num_bytes == 2 || num_bytes == 4 || num_bytes == 8);
  char* cptr = static_cast<char*>(ptr);
  int halfsize = num_bytes / 2;
  for (int i = 0; i < halfsize; ++i) {
    char tmp = cptr[i];
    cptr[i] = cptr[num_bytes - 1 - i];
    cptr[num_bytes - 1 - i] = tmp;
  }
}

// Reverse the order of bytes in a 16 bit quantity for big/little-endian switch.
inline void Reverse16(void* ptr) {
  ReverseN(ptr, 2);
}

// Reverse the order of bytes in a 32 bit quantity for big/little-endian switch.
inline void Reverse32(void* ptr) {
  ReverseN(ptr, 4);
}

// Reverse the order of bytes in a 64 bit quantity for big/little-endian switch.
inline void Reverse64(void* ptr) {
  ReverseN(ptr, 8);
}

//------------------------------

///integer coordinate
class ICOORD
{
	friend class FCOORD;

public:
	///empty constructor
	ICOORD() {
		xcoord = ycoord = 0;       //default zero
	}
	///constructor
	///@param xin x value
	///@param yin y value
	ICOORD(int16_t xin,
		int16_t yin) {
		xcoord = xin;
		ycoord = yin;
	}
	///destructor
	~ICOORD() = default;

	///access function
	int16_t x() const {
		return xcoord;
	}
	///access_function
	int16_t y() const {
		return ycoord;
	}

	///rewrite function
	void set_x(int16_t xin) {
		xcoord = xin;              //write new value
	}
	///rewrite function
	void set_y(int16_t yin) {  //value to set
		ycoord = yin;
	}

	/// Set from the given x,y, shrinking the vector to fit if needed.
	void set_with_shrink(int x, int y) {
		// Fit the vector into an ICOORD, which is 16 bit.
		int factor = 1;
		int max_extent = std::max(abs(x), abs(y));
		if (max_extent > INT16_MAX)
			factor = max_extent / INT16_MAX + 1;
		xcoord = x / factor;
		ycoord = y / factor;
	}

	///find sq length
	float sqlength() const {
		return xcoord * xcoord + ycoord * ycoord;
	}

	///find length
	float length() const {
		return std::sqrt(sqlength());
	}

	///sq dist between pts
	float pt_to_pt_sqdist(const ICOORD &pt) const {
		ICOORD gap;

		gap.xcoord = xcoord - pt.xcoord;
		gap.ycoord = ycoord - pt.ycoord;
		return gap.sqlength();
	}

	///Distance between pts
	float pt_to_pt_dist(const ICOORD &pt) const {
		return std::sqrt(pt_to_pt_sqdist(pt));
	}

	///find angle
	float angle() const {
		return std::atan2(ycoord, xcoord);
	}

	///test equality
	bool operator== (const ICOORD & other) const {
		return xcoord == other.xcoord && ycoord == other.ycoord;
	}
	///test inequality
	bool operator!= (const ICOORD & other) const {
		return xcoord != other.xcoord || ycoord != other.ycoord;
	}
	///rotate 90 deg anti
	friend ICOORD operator! (const ICOORD &);
	///unary minus
	friend ICOORD operator- (const ICOORD &);
	///add
	friend ICOORD operator+ (const ICOORD &, const ICOORD &);
	///add
	friend ICOORD & operator+= (ICOORD &, const ICOORD &);
	///subtract
	friend ICOORD operator- (const ICOORD &, const ICOORD &);
	///subtract
	friend ICOORD & operator-= (ICOORD &, const ICOORD &);
	///scalar product
	friend int32_t operator% (const ICOORD &, const ICOORD &);
	///cross product
	friend int32_t operator *(const ICOORD &,
		const ICOORD &);
	///multiply
	friend ICOORD operator *(const ICOORD &,
		int16_t);
	///multiply
	friend ICOORD operator *(int16_t,
		const ICOORD &);
	///multiply
	friend ICOORD & operator*= (ICOORD &, int16_t);
	///divide
	friend ICOORD operator/ (const ICOORD &, int16_t);
	///divide
	friend ICOORD & operator/= (ICOORD &, int16_t);
	///rotate
	///@param vec by vector
	void rotate(const FCOORD& vec);

	/// Setup for iterating over the pixels in a vector by the well-known
	/// Bresenham rendering algorithm.
	/// Starting with major/2 in the accumulator, on each step move by
	/// major_step, and then add minor to the accumulator. When
	/// accumulator >= major subtract major and also move by minor_step.
	void setup_render(ICOORD* major_step, ICOORD* minor_step,
		int* major, int* minor) const {
		int abs_x = abs(xcoord);
		int abs_y = abs(ycoord);
		if (abs_x >= abs_y) {
			// X-direction is major.
			major_step->xcoord = sign(xcoord);
			major_step->ycoord = 0;
			minor_step->xcoord = 0;
			minor_step->ycoord = sign(ycoord);
			*major = abs_x;
			*minor = abs_y;
		}
		else {
			// Y-direction is major.
			major_step->xcoord = 0;
			major_step->ycoord = sign(ycoord);
			minor_step->xcoord = sign(xcoord);
			minor_step->ycoord = 0;
			*major = abs_y;
			*minor = abs_x;
		}
	}

	// Writes to the given file. Returns false in case of error.
	bool Serialize(FILE* fp) const {
		return tesseract::Serialize(fp, &xcoord) &&
			tesseract::Serialize(fp, &ycoord);
	}
	// Reads from the given file. Returns false in case of error.
	// If swap is true, assumes a big/little-endian swap is needed.
	bool DeSerialize(bool swap, FILE* fp) {
		if (!tesseract::DeSerialize(fp, &xcoord)) return false;
		if (!tesseract::DeSerialize(fp, &ycoord)) return false;
		if (swap) {
			ReverseN(&xcoord, sizeof(xcoord));
			ReverseN(&ycoord, sizeof(ycoord));
		}
		return true;
	}
protected:
	int16_t xcoord;                ///< x value
	int16_t ycoord;                ///< y value
};


class FCOORD {
public:
	///empty constructor
	FCOORD() = default;
	///constructor
	///@param xvalue x value
	///@param yvalue y value
	FCOORD(float xvalue,
		float yvalue) {
		xcoord = xvalue;           //set coords
		ycoord = yvalue;
	}
	FCOORD(                  //make from ICOORD
		ICOORD icoord) {  //coords to set
		xcoord = icoord.xcoord;
		ycoord = icoord.ycoord;
	}

	float x() const {  //get coords
		return xcoord;
	}
	float y() const {
		return ycoord;
	}
	///rewrite function
	void set_x(float xin) {
		xcoord = xin;              //write new value
	}
	///rewrite function
	void set_y(float yin) {  //value to set
		ycoord = yin;
	}

	///find sq length
	float sqlength() const {
		return xcoord * xcoord + ycoord * ycoord;
	}

	///find length
	float length() const {
		return std::sqrt(sqlength());
	}

	///sq dist between pts
	float pt_to_pt_sqdist(const FCOORD &pt) const {
		FCOORD gap;

		gap.xcoord = xcoord - pt.xcoord;
		gap.ycoord = ycoord - pt.ycoord;
		return gap.sqlength();
	}

	///Distance between pts
	float pt_to_pt_dist(const FCOORD &pt) const {
		return std::sqrt(pt_to_pt_sqdist(pt));
	}

	///find angle
	float angle() const {
		return std::atan2(ycoord, xcoord);
	}
	// Returns the standard feature direction corresponding to this.
	// See binary_angle_plus_pi below for a description of the direction.
	uint8_t to_direction() const {
		return binary_angle_plus_pi(angle());
	}
	// Sets this with a unit vector in the given standard feature direction.
	void from_direction(uint8_t direction) {
		double radians = angle_from_direction(direction);
		xcoord = cos(radians);
		ycoord = sin(radians);
	}

	// Converts an angle in radians (from ICOORD::angle or FCOORD::angle) to a
	// standard feature direction as an unsigned angle in 256ths of a circle
	// measured anticlockwise from (-1, 0).
	static uint8_t binary_angle_plus_pi(double radians) {
		return Modulo(IntCastRounded((radians + M_PI) * 128.0 / M_PI), 256);
	}
	// Inverse of binary_angle_plus_pi returns an angle in radians for the
	// given standard feature direction.
	static double angle_from_direction(uint8_t direction) {
		return direction * M_PI / 128.0 - M_PI;
	}
	// Returns the point on the given line nearest to this, ie the point such
	// that the vector point->this is perpendicular to the line.
	// The line is defined as a line_point and a dir_vector for its direction.
	// dir_vector need not be a unit vector.
	FCOORD nearest_pt_on_line(const FCOORD& line_point,
		const FCOORD& dir_vector) const {
		FCOORD point_vector(*this - line_point);
		// The dot product (%) is |dir_vector||point_vector|cos theta, so dividing by
		// the square of the length of dir_vector gives us the fraction of dir_vector
		// to add to line1 to get the appropriate point, so
		// result = line1 + lambda dir_vector.
		double lambda = point_vector % dir_vector / dir_vector.sqlength();
		return line_point + (dir_vector * lambda);
	}

	///Convert to unit vec
	bool normalise() {  //Convert to unit vec
		float len = length();

		if (len < 0.0000000001) {
			return false;
		}
		xcoord /= len;
		ycoord /= len;
		return true;
	}

	///test equality
	bool operator== (const FCOORD & other) {
		return xcoord == other.xcoord && ycoord == other.ycoord;
	}
	///test inequality
	bool operator!= (const FCOORD & other) {
		return xcoord != other.xcoord || ycoord != other.ycoord;
	}
	///rotate 90 deg anti
	friend FCOORD operator! (const FCOORD &);
	///unary minus
	friend FCOORD operator- (const FCOORD &);
	///add
	friend FCOORD operator+ (const FCOORD &, const FCOORD &);
	///add
	friend FCOORD & operator+= (FCOORD &, const FCOORD &);
	///subtract
	friend FCOORD operator- (const FCOORD &, const FCOORD &);
	///subtract
	friend FCOORD & operator-= (FCOORD &, const FCOORD &);
	///scalar product
	friend float operator% (const FCOORD &, const FCOORD &);
	///cross product
	friend float operator *(const FCOORD &, const FCOORD &);
	///multiply
	friend FCOORD operator *(const FCOORD &, float);
	///multiply
	friend FCOORD operator *(float, const FCOORD &);

	///multiply
	friend FCOORD & operator*= (FCOORD &, float);
	///divide
	friend FCOORD operator/ (const FCOORD &, float);
	///rotate
	///@param vec by vector
	void rotate(const FCOORD vec);
	// unrotate - undo a rotate(vec)
	// @param vec by vector
	void unrotate(const FCOORD &vec);
	///divide
	friend FCOORD & operator/= (FCOORD &, float);

private:
	float xcoord;                //2 floating coords
	float ycoord;
};


/**********************************************************************
 * operator!
 *
 * Rotate an ICOORD 90 degrees anticlockwise.
 **********************************************************************/

inline ICOORD
operator! (                      //rotate 90 deg anti
	const ICOORD & src               //thing to rotate
	) {
	ICOORD result;                 //output

	result.xcoord = -src.ycoord;
	result.ycoord = src.xcoord;
	return result;
}


/**********************************************************************
 * operator-
 *
 * Unary minus of an ICOORD.
 **********************************************************************/

inline ICOORD
operator- (                      //unary minus
	const ICOORD & src               //thing to minus
	) {
	ICOORD result;                 //output

	result.xcoord = -src.xcoord;
	result.ycoord = -src.ycoord;
	return result;
}


/**********************************************************************
 * operator+
 *
 * Add 2 ICOORDS.
 **********************************************************************/

inline ICOORD
operator+ (                      //sum vectors
	const ICOORD & op1,              //operands
	const ICOORD & op2) {
	ICOORD sum;                    //result

	sum.xcoord = op1.xcoord + op2.xcoord;
	sum.ycoord = op1.ycoord + op2.ycoord;
	return sum;
}


/**********************************************************************
 * operator+=
 *
 * Add 2 ICOORDS.
 **********************************************************************/

inline ICOORD &
operator+= (                     //sum vectors
	ICOORD & op1,                    //operands
	const ICOORD & op2) {
	op1.xcoord += op2.xcoord;
	op1.ycoord += op2.ycoord;
	return op1;
}


/**********************************************************************
 * operator-
 *
 * Subtract 2 ICOORDS.
 **********************************************************************/

inline ICOORD
operator- (                      //subtract vectors
	const ICOORD & op1,              //operands
	const ICOORD & op2) {
	ICOORD sum;                    //result

	sum.xcoord = op1.xcoord - op2.xcoord;
	sum.ycoord = op1.ycoord - op2.ycoord;
	return sum;
}


/**********************************************************************
 * operator-=
 *
 * Subtract 2 ICOORDS.
 **********************************************************************/

inline ICOORD &
operator-= (                     //subtract vectors
	ICOORD & op1,                    //operands
	const ICOORD & op2) {
	op1.xcoord -= op2.xcoord;
	op1.ycoord -= op2.ycoord;
	return op1;
}


/**********************************************************************
 * operator%
 *
 * Scalar product of 2 ICOORDS.
 **********************************************************************/

inline int32_t
operator% (                      //scalar product
	const ICOORD & op1,              //operands
	const ICOORD & op2) {
	return op1.xcoord * op2.xcoord + op1.ycoord * op2.ycoord;
}


/**********************************************************************
 * operator*
 *
 * Cross product of 2 ICOORDS.
 **********************************************************************/

inline int32_t operator *(                    //cross product
	const ICOORD &op1,  //operands
	const ICOORD &op2) {
	return op1.xcoord * op2.ycoord - op1.ycoord * op2.xcoord;
}


/**********************************************************************
 * operator*
 *
 * Scalar multiply of an ICOORD.
 **********************************************************************/

inline ICOORD operator *(                    //scalar multiply
	const ICOORD &op1,  //operands
	int16_t scale) {
	ICOORD result;                 //output

	result.xcoord = op1.xcoord * scale;
	result.ycoord = op1.ycoord * scale;
	return result;
}


inline ICOORD operator *(                   //scalar multiply
	int16_t scale,
	const ICOORD &op1  //operands
	) {
	ICOORD result;                 //output

	result.xcoord = op1.xcoord * scale;
	result.ycoord = op1.ycoord * scale;
	return result;
}


/**********************************************************************
 * operator*=
 *
 * Scalar multiply of an ICOORD.
 **********************************************************************/

inline ICOORD &
operator*= (                     //scalar multiply
	ICOORD & op1,                    //operands
	int16_t scale) {
	op1.xcoord *= scale;
	op1.ycoord *= scale;
	return op1;
}


/**********************************************************************
 * operator/
 *
 * Scalar divide of an ICOORD.
 **********************************************************************/

inline ICOORD
operator/ (                      //scalar divide
	const ICOORD & op1,              //operands
	int16_t scale) {
	ICOORD result;                 //output

	result.xcoord = op1.xcoord / scale;
	result.ycoord = op1.ycoord / scale;
	return result;
}


/**********************************************************************
 * operator/=
 *
 * Scalar divide of an ICOORD.
 **********************************************************************/

inline ICOORD &
operator/= (                     //scalar divide
	ICOORD & op1,                    //operands
	int16_t scale) {
	op1.xcoord /= scale;
	op1.ycoord /= scale;
	return op1;
}


/**********************************************************************
 * ICOORD::rotate
 *
 * Rotate an ICOORD by the given (normalized) (cos,sin) vector.
 **********************************************************************/

inline void ICOORD::rotate(  //rotate by vector
	const FCOORD& vec) {
	auto tmp = static_cast<int16_t>(std::floor(xcoord * vec.x() -
		ycoord * vec.y() + 0.5f));
	ycoord = static_cast<int16_t>(std::floor(ycoord * vec.x() +
		xcoord * vec.y() + 0.5f));
	xcoord = tmp;
}


/**********************************************************************
 * operator!
 *
 * Rotate an FCOORD 90 degrees anticlockwise.
 **********************************************************************/

inline FCOORD
operator! (                      //rotate 90 deg anti
	const FCOORD & src               //thing to rotate
	) {
	FCOORD result;                 //output

	result.xcoord = -src.ycoord;
	result.ycoord = src.xcoord;
	return result;
}


/**********************************************************************
 * operator-
 *
 * Unary minus of an FCOORD.
 **********************************************************************/

inline FCOORD
operator- (                      //unary minus
	const FCOORD & src               //thing to minus
	) {
	FCOORD result;                 //output

	result.xcoord = -src.xcoord;
	result.ycoord = -src.ycoord;
	return result;
}


/**********************************************************************
 * operator+
 *
 * Add 2 FCOORDS.
 **********************************************************************/

inline FCOORD
operator+ (                      //sum vectors
	const FCOORD & op1,              //operands
	const FCOORD & op2) {
	FCOORD sum;                    //result

	sum.xcoord = op1.xcoord + op2.xcoord;
	sum.ycoord = op1.ycoord + op2.ycoord;
	return sum;
}


/**********************************************************************
 * operator+=
 *
 * Add 2 FCOORDS.
 **********************************************************************/

inline FCOORD &
operator+= (                     //sum vectors
	FCOORD & op1,                    //operands
	const FCOORD & op2) {
	op1.xcoord += op2.xcoord;
	op1.ycoord += op2.ycoord;
	return op1;
}


/**********************************************************************
 * operator-
 *
 * Subtract 2 FCOORDS.
 **********************************************************************/

inline FCOORD
operator- (                      //subtract vectors
	const FCOORD & op1,              //operands
	const FCOORD & op2) {
	FCOORD sum;                    //result

	sum.xcoord = op1.xcoord - op2.xcoord;
	sum.ycoord = op1.ycoord - op2.ycoord;
	return sum;
}


/**********************************************************************
 * operator-=
 *
 * Subtract 2 FCOORDS.
 **********************************************************************/

inline FCOORD &
operator-= (                     //subtract vectors
	FCOORD & op1,                    //operands
	const FCOORD & op2) {
	op1.xcoord -= op2.xcoord;
	op1.ycoord -= op2.ycoord;
	return op1;
}


/**********************************************************************
 * operator%
 *
 * Scalar product of 2 FCOORDS.
 **********************************************************************/

inline float
operator% (                      //scalar product
	const FCOORD & op1,              //operands
	const FCOORD & op2) {
	return op1.xcoord * op2.xcoord + op1.ycoord * op2.ycoord;
}


/**********************************************************************
 * operator*
 *
 * Cross product of 2 FCOORDS.
 **********************************************************************/

inline float operator *(                    //cross product
	const FCOORD &op1,  //operands
	const FCOORD &op2) {
	return op1.xcoord * op2.ycoord - op1.ycoord * op2.xcoord;
}


/**********************************************************************
 * operator*
 *
 * Scalar multiply of an FCOORD.
 **********************************************************************/

inline FCOORD operator *(                    //scalar multiply
	const FCOORD &op1,  //operands
	float scale) {
	FCOORD result;                 //output

	result.xcoord = op1.xcoord * scale;
	result.ycoord = op1.ycoord * scale;
	return result;
}


inline FCOORD operator *(                   //scalar multiply
	float scale,
	const FCOORD &op1  //operands
	) {
	FCOORD result;                 //output

	result.xcoord = op1.xcoord * scale;
	result.ycoord = op1.ycoord * scale;
	return result;
}


/**********************************************************************
 * operator*=
 *
 * Scalar multiply of an FCOORD.
 **********************************************************************/

inline FCOORD &
operator*= (                     //scalar multiply
	FCOORD & op1,                    //operands
	float scale) {
	op1.xcoord *= scale;
	op1.ycoord *= scale;
	return op1;
}


/**********************************************************************
 * operator/
 *
 * Scalar divide of an FCOORD.
 **********************************************************************/

inline FCOORD
operator/ (                      //scalar divide
	const FCOORD & op1,              //operands
	float scale) {
	FCOORD result;                 //output
	ASSERT_HOST(scale != 0.0f);
	result.xcoord = op1.xcoord / scale;
	result.ycoord = op1.ycoord / scale;
	return result;
}


/**********************************************************************
 * operator/=
 *
 * Scalar divide of an FCOORD.
 **********************************************************************/

inline FCOORD &
operator/= (                     //scalar divide
	FCOORD & op1,                    //operands
	float scale) {
	ASSERT_HOST(scale != 0.0f);
	op1.xcoord /= scale;
	op1.ycoord /= scale;
	return op1;
}


/**********************************************************************
 * rotate
 *
 * Rotate an FCOORD by the given (normalized) (cos,sin) vector.
 **********************************************************************/

inline void FCOORD::rotate(  //rotate by vector
	const FCOORD vec) {
	float tmp;

	tmp = xcoord * vec.x() - ycoord * vec.y();
	ycoord = ycoord * vec.x() + xcoord * vec.y();
	xcoord = tmp;
}

inline void FCOORD::unrotate(const FCOORD& vec) {
	rotate(FCOORD(vec.x(), -vec.y()));
}


//--------------------------------
// TBOX
class TBOX {  // bounding box
public:
	TBOX() :       // empty constructor making a null box
		bot_left(INT16_MAX, INT16_MAX), top_right(-INT16_MAX, -INT16_MAX) {
	}

	TBOX(          // constructor
		const ICOORD pt1,   // one corner
		const ICOORD pt2);  // the other corner

	TBOX(                    // constructor
		int16_t left, int16_t bottom, int16_t right, int16_t top);

	TBOX(  // box around FCOORD
		const FCOORD pt);

	bool null_box() const {  // Is box null
		return ((left() >= right()) || (top() <= bottom()));
	}

	bool operator==(const TBOX& other) const {
		return bot_left == other.bot_left && top_right == other.top_right;
	}

	int16_t top() const {  // coord of top
		return top_right.y();
	}
	void set_top(int y) {
		top_right.set_y(y);
	}

	int16_t bottom() const {  // coord of bottom
		return bot_left.y();
	}
	void set_bottom(int y) {
		bot_left.set_y(y);
	}

	int16_t left() const {  // coord of left
		return bot_left.x();
	}
	void set_left(int x) {
		bot_left.set_x(x);
	}

	int16_t right() const {  // coord of right
		return top_right.x();
	}
	void set_right(int x) {
		top_right.set_x(x);
	}
	int x_middle() const {
		return (bot_left.x() + top_right.x()) / 2;
	}
	int y_middle() const {
		return (bot_left.y() + top_right.y()) / 2;
	}

	const ICOORD &botleft() const {  // access function
		return bot_left;
	}

	ICOORD botright() const {  // ~ access function
		return ICOORD(top_right.x(), bot_left.y());
	}

	ICOORD topleft() const {  // ~ access function
		return ICOORD(bot_left.x(), top_right.y());
	}

	const ICOORD &topright() const {  // access function
		return top_right;
	}

	int16_t height() const {  // how high is it?
		if (!null_box())
			return top_right.y() - bot_left.y();
		else
			return 0;
	}

	int16_t width() const {  // how high is it?
		if (!null_box())
			return top_right.x() - bot_left.x();
		else
			return 0;
	}

	int32_t area() const {  // what is the area?
		if (!null_box())
			return width() * height();
		else
			return 0;
	}

	// Pads the box on either side by the supplied x,y pad amounts.
	// NO checks for exceeding any bounds like 0 or an image size.
	void pad(int xpad, int ypad) {
		ICOORD pad(xpad, ypad);
		bot_left -= pad;
		top_right += pad;
	}

	void move_bottom_edge(                  // move one edge
		const int16_t y) {  // by +/- y
		bot_left += ICOORD(0, y);
	}

	void move_left_edge(                  // move one edge
		const int16_t x) {  // by +/- x
		bot_left += ICOORD(x, 0);
	}

	void move_right_edge(                  // move one edge
		const int16_t x) {  // by +/- x
		top_right += ICOORD(x, 0);
	}

	void move_top_edge(                  // move one edge
		const int16_t y) {  // by +/- y
		top_right += ICOORD(0, y);
	}

	void move(                     // move box
		const ICOORD vec) {  // by vector
		bot_left += vec;
		top_right += vec;
	}

	void move(                     // move box
		const FCOORD vec) {  // by float vector
		bot_left.set_x(static_cast<int16_t>(std::floor(bot_left.x() + vec.x())));
		// round left
		bot_left.set_y(static_cast<int16_t>(std::floor(bot_left.y() + vec.y())));
		// round down
		top_right.set_x(static_cast<int16_t>(std::ceil(top_right.x() + vec.x())));
		// round right
		top_right.set_y(static_cast<int16_t>(std::ceil(top_right.y() + vec.y())));
		// round up
	}

	void scale(                  // scale box
		const float f) {  // by multiplier
// round left
		bot_left.set_x(static_cast<int16_t>(std::floor(bot_left.x() * f)));
		// round down
		bot_left.set_y(static_cast<int16_t>(std::floor(bot_left.y() * f)));
		// round right
		top_right.set_x(static_cast<int16_t>(std::ceil(top_right.x() * f)));
		// round up
		top_right.set_y(static_cast<int16_t>(std::ceil(top_right.y() * f)));
	}
	void scale(                     // scale box
		const FCOORD vec) {  // by float vector
		bot_left.set_x(static_cast<int16_t>(std::floor(bot_left.x() * vec.x())));
		bot_left.set_y(static_cast<int16_t>(std::floor(bot_left.y() * vec.y())));
		top_right.set_x(static_cast<int16_t>(std::ceil(top_right.x() * vec.x())));
		top_right.set_y(static_cast<int16_t>(std::ceil(top_right.y() * vec.y())));
	}

	// rotate doesn't enlarge the box - it just rotates the bottom-left
	// and top-right corners. Use rotate_large if you want to guarantee
	// that all content is contained within the rotated box.
	void rotate(const FCOORD& vec) {  // by vector
		bot_left.rotate(vec);
		top_right.rotate(vec);
		*this = TBOX(bot_left, top_right);
	}
	// rotate_large constructs the containing bounding box of all 4
	// corners after rotating them. It therefore guarantees that all
	// original content is contained within, but also slightly enlarges the box.
	void rotate_large(const FCOORD& vec);

	bool contains(  // is pt inside box
		const FCOORD pt) const;

	bool contains(  // is box inside box
		const TBOX &box) const;

	bool overlap(  // do boxes overlap
		const TBOX &box) const;

	bool major_overlap(  // do boxes overlap more than half
		const TBOX &box) const;

	// Do boxes overlap on x axis.
	bool x_overlap(const TBOX &box) const;

	// Return the horizontal gap between the boxes. If the boxes
	// overlap horizontally then the return value is negative, indicating
	// the amount of the overlap.
	int x_gap(const TBOX& box) const {
		return std::max(bot_left.x(), box.bot_left.x()) -
			std::min(top_right.x(), box.top_right.x());
	}

	// Return the vertical gap between the boxes. If the boxes
	// overlap vertically then the return value is negative, indicating
	// the amount of the overlap.
	int y_gap(const TBOX& box) const {
		return std::max(bot_left.y(), box.bot_left.y()) -
			std::min(top_right.y(), box.top_right.y());
	}

	// Do boxes overlap on x axis by more than
	// half of the width of the narrower box.
	bool major_x_overlap(const TBOX &box) const;

	// Do boxes overlap on y axis.
	bool y_overlap(const TBOX &box) const;

	// Do boxes overlap on y axis by more than
	// half of the height of the shorter box.
	bool major_y_overlap(const TBOX &box) const;

	// fraction of current box's area covered by other
	double overlap_fraction(const TBOX &box) const;

	// fraction of the current box's projected area covered by the other's
	double x_overlap_fraction(const TBOX& box) const;

	// fraction of the current box's projected area covered by the other's
	double y_overlap_fraction(const TBOX& box) const;

	// Returns true if the boxes are almost equal on x axis.
	bool x_almost_equal(const TBOX &box, int tolerance) const;

	// Returns true if the boxes are almost equal
	bool almost_equal(const TBOX &box, int tolerance) const;

	TBOX intersection(  // shared area box
		const TBOX &box) const;

	TBOX bounding_union(  // box enclosing both
		const TBOX &box) const;

	// Sets the box boundaries to the given coordinates.
	void set_to_given_coords(int x_min, int y_min, int x_max, int y_max) {
		bot_left.set_x(x_min);
		bot_left.set_y(y_min);
		top_right.set_x(x_max);
		top_right.set_y(y_max);
	}

	// NOT-USE for locketk9
	//void print() const {  // print
	//	tprintf("Bounding box=(%d,%d)->(%d,%d)\n",
	//		left(), bottom(), right(), top());
	//}
	//// Appends the bounding box as (%d,%d)->(%d,%d) to a STRING.
	//void print_to_str(STRING *str) const;

#ifndef GRAPHICS_DISABLED
	void plot(                    // use current settings
		ScrollView* fd) const {  // where to paint
		fd->Rectangle(bot_left.x(), bot_left.y(), top_right.x(),
			top_right.y());
	}

	void plot(                              // paint box
		ScrollView* fd,                    // where to paint
		ScrollView::Color fill_colour,           // colour for inside
		ScrollView::Color border_colour) const;  // colour for border
#endif
	// Writes to the given file. Returns false in case of error.
	bool Serialize(FILE* fp) const;
	// Reads from the given file. Returns false in case of error.
	// If swap is true, assumes a big/little-endian swap is needed.
	bool DeSerialize(bool swap, FILE* fp);

	friend TBOX& operator+=(TBOX&, const TBOX&);
	// in place union
	friend TBOX& operator&=(TBOX&, const TBOX&);
	// in place intersection

private:
	ICOORD bot_left;             // bottom left corner
	ICOORD top_right;            // top right corner
};


/**********************************************************************
 * TBOX::TBOX()  Constructor from 1 FCOORD
 *
 **********************************************************************/

inline TBOX::TBOX(   // constructor
	const FCOORD pt  // floating centre
) {
	bot_left = ICOORD(static_cast<int16_t>(std::floor(pt.x())),
		static_cast<int16_t>(std::floor(pt.y())));
	top_right = ICOORD(static_cast<int16_t>(std::ceil(pt.x())),
		static_cast<int16_t>(std::ceil(pt.y())));
}


/**********************************************************************
 * TBOX::contains()  Is point within box
 *
 **********************************************************************/

inline bool TBOX::contains(const FCOORD pt) const {
	return ((pt.x() >= bot_left.x()) &&
		(pt.x() <= top_right.x()) &&
		(pt.y() >= bot_left.y()) && (pt.y() <= top_right.y()));
}


/**********************************************************************
 * TBOX::contains()  Is box within box
 *
 **********************************************************************/

inline bool TBOX::contains(const TBOX &box) const {
	return (contains(box.bot_left) && contains(box.top_right));
}


/**********************************************************************
 * TBOX::overlap()  Do two boxes overlap?
 *
 **********************************************************************/

inline bool TBOX::overlap(  // do boxes overlap
	const TBOX &box) const {
	return ((box.bot_left.x() <= top_right.x()) &&
		(box.top_right.x() >= bot_left.x()) &&
		(box.bot_left.y() <= top_right.y()) &&
		(box.top_right.y() >= bot_left.y()));
}

/**********************************************************************
 * TBOX::major_overlap()  Do two boxes overlap by at least half of the smallest?
 *
 **********************************************************************/

inline bool TBOX::major_overlap(  // Do boxes overlap more that half.
	const TBOX &box) const {
	int overlap = std::min(box.top_right.x(), top_right.x());
	overlap -= std::max(box.bot_left.x(), bot_left.x());
	overlap += overlap;
	if (overlap < std::min(box.width(), width()))
		return false;
	overlap = std::min(box.top_right.y(), top_right.y());
	overlap -= std::max(box.bot_left.y(), bot_left.y());
	overlap += overlap;
	if (overlap < std::min(box.height(), height()))
		return false;
	return true;
}

/**********************************************************************
 * TBOX::overlap_fraction()  Fraction of area covered by the other box
 *
 **********************************************************************/

inline double TBOX::overlap_fraction(const TBOX &box) const {
	double fraction = 0.0;
	if (this->area()) {
		fraction = this->intersection(box).area() * 1.0 / this->area();
	}
	return fraction;
}

/**********************************************************************
 * TBOX::x_overlap()  Do two boxes overlap on x-axis
 *
 **********************************************************************/

inline bool TBOX::x_overlap(const TBOX &box) const {
	return ((box.bot_left.x() <= top_right.x()) &&
		(box.top_right.x() >= bot_left.x()));
}

/**********************************************************************
 * TBOX::major_x_overlap()  Do two boxes overlap by more than half the
 *                          width of the narrower box on the x-axis
 *
 **********************************************************************/

inline bool TBOX::major_x_overlap(const TBOX &box) const {
	int16_t overlap = box.width();
	if (this->left() > box.left()) {
		overlap -= this->left() - box.left();
	}
	if (this->right() < box.right()) {
		overlap -= box.right() - this->right();
	}
	return (overlap >= box.width() / 2 || overlap >= this->width() / 2);
}

/**********************************************************************
 * TBOX::y_overlap()  Do two boxes overlap on y-axis
 *
 **********************************************************************/

inline bool TBOX::y_overlap(const TBOX &box) const {
	return ((box.bot_left.y() <= top_right.y()) &&
		(box.top_right.y() >= bot_left.y()));
}

/**********************************************************************
 * TBOX::major_y_overlap()  Do two boxes overlap by more than half the
 *                          height of the shorter box on the y-axis
 *
 **********************************************************************/

inline bool TBOX::major_y_overlap(const TBOX &box) const {
	int16_t overlap = box.height();
	if (this->bottom() > box.bottom()) {
		overlap -= this->bottom() - box.bottom();
	}
	if (this->top() < box.top()) {
		overlap -= box.top() - this->top();
	}
	return (overlap >= box.height() / 2 || overlap >= this->height() / 2);
}

/**********************************************************************
 * TBOX::x_overlap_fraction() Calculates the horizontal overlap of the
 *                            given boxes as a fraction of this boxes
 *                            width.
 *
 **********************************************************************/

inline double TBOX::x_overlap_fraction(const TBOX& other) const {
	int low = std::max(left(), other.left());
	int high = std::min(right(), other.right());
	int width = right() - left();
	if (width == 0) {
		int x = left();
		if (other.left() <= x && x <= other.right())
			return 1.0;
		else
			return 0.0;
	}
	else {
		return std::max(0.0, static_cast<double>(high - low) / width);
	}
}

/**********************************************************************
 * TBOX::y_overlap_fraction() Calculates the vertical overlap of the
 *                            given boxes as a fraction of this boxes
 *                            height.
 *
 **********************************************************************/

inline double TBOX::y_overlap_fraction(const TBOX& other) const {
	int low = std::max(bottom(), other.bottom());
	int high = std::min(top(), other.top());
	int height = top() - bottom();
	if (height == 0) {
		int y = bottom();
		if (other.bottom() <= y && y <= other.top())
			return 1.0;
		else
			return 0.0;
	}
	else {
		return std::max(0.0, static_cast<double>(high - low) / height);
	}
}

//--------------------------------

//--------------------------------

// A generic class to hold a 2-D matrix with entries of type T, but can also
// act as a base class for other implementations, such as a triangular or
// banded matrix.
template <class T>
class GENERIC_2D_ARRAY {
public:
	// Initializes the array size, and empty element, but cannot allocate memory
	// for the subclasses or initialize because calls to the num_elements
	// member will be routed to the base class implementation. Subclasses can
	// either pass the memory in, or allocate after by calling Resize().
	GENERIC_2D_ARRAY(int dim1, int dim2, const T& empty, T* array)
		: empty_(empty), dim1_(dim1), dim2_(dim2), array_(array) {
		size_allocated_ = dim1 * dim2;
	}
	// Original constructor for a full rectangular matrix DOES allocate memory
	// and initialize it to empty.
	GENERIC_2D_ARRAY(int dim1, int dim2, const T& empty)
		: empty_(empty), dim1_(dim1), dim2_(dim2) {
		int new_size = dim1 * dim2;
		array_ = new T[new_size];
		size_allocated_ = new_size;
		for (int i = 0; i < size_allocated_; ++i)
			array_[i] = empty_;
	}
	// Default constructor for array allocation. Use Resize to set the size.
	GENERIC_2D_ARRAY()
		: array_(nullptr), empty_(static_cast<T>(0)), dim1_(0), dim2_(0),
		size_allocated_(0) {
	}
	GENERIC_2D_ARRAY(const GENERIC_2D_ARRAY<T>& src)
		: array_(nullptr), empty_(static_cast<T>(0)), dim1_(0), dim2_(0),
		size_allocated_(0) {
		*this = src;
	}
	virtual ~GENERIC_2D_ARRAY() { delete[] array_; }

	void operator=(const GENERIC_2D_ARRAY<T>& src) {
		ResizeNoInit(src.dim1(), src.dim2());
		int size = num_elements();
		if (size > 0) {
			memcpy(array_, src.array_, size * sizeof(array_[0]));
		}
	}

	// Reallocates the array to the given size. Does not keep old data, but does
	// not initialize the array either.
	// The allocated memory is expanded on the end by pad, allowing deliberate
	// access beyond the bounds of the array.
	void ResizeNoInit(int size1, int size2, int pad = 0) {
		int new_size = size1 * size2 + pad;
		if (new_size > size_allocated_) {
			delete[] array_;
			array_ = new T[new_size];
			size_allocated_ = new_size;
		}
		dim1_ = size1;
		dim2_ = size2;
		// Fill the padding data so it isn't uninitialized.
		for (int i = size1 * size2; i < new_size; ++i) array_[i] = empty_;
	}

	// Reallocate the array to the given size. Does not keep old data.
	void Resize(int size1, int size2, const T& empty) {
		empty_ = empty;
		ResizeNoInit(size1, size2);
		Clear();
	}

	// Reallocate the array to the given size, keeping old data.
	void ResizeWithCopy(int size1, int size2) {
		if (size1 != dim1_ || size2 != dim2_) {
			int new_size = size1 * size2;
			T* new_array = new T[new_size];
			for (int col = 0; col < size1; ++col) {
				for (int row = 0; row < size2; ++row) {
					int old_index = col * dim2() + row;
					int new_index = col * size2 + row;
					if (col < dim1_ && row < dim2_) {
						new_array[new_index] = array_[old_index];
					}
					else {
						new_array[new_index] = empty_;
					}
				}
			}
			delete[] array_;
			array_ = new_array;
			dim1_ = size1;
			dim2_ = size2;
			size_allocated_ = new_size;
		}
	}

	// Sets all the elements of the array to the empty value.
	void Clear() {
		int total_size = num_elements();
		for (int i = 0; i < total_size; ++i)
			array_[i] = empty_;
	}

	// Writes to the given file. Returns false in case of error.
	// Only works with bitwise-serializeable types!
	bool Serialize(FILE* fp) const {
		if (!SerializeSize(fp)) return false;
		if (!tesseract::Serialize(fp, &empty_)) return false;
		int size = num_elements();
		return tesseract::Serialize(fp, &array_[0], size);
	}

	//bool Serialize(tesseract::TFile* fp) const {
	//	if (!SerializeSize(fp)) return false;
	//	if (!fp->Serialize(&empty_)) return false;
	//	int size = num_elements();
	//	return fp->Serialize(&array_[0], size);
	//}

	// Reads from the given file. Returns false in case of error.
	// Only works with bitwise-serializeable types!
	// If swap is true, assumes a big/little-endian swap is needed.
	bool DeSerialize(bool swap, FILE* fp) {
		if (!DeSerializeSize(swap, fp)) return false;
		if (!tesseract::DeSerialize(fp, &empty_)) return false;
		if (swap) ReverseN(&empty_, sizeof(empty_));
		int size = num_elements();
		if (!tesseract::DeSerialize(fp, &array_[0], size)) return false;
		if (swap) {
			for (int i = 0; i < size; ++i)
				ReverseN(&array_[i], sizeof(array_[i]));
		}
		return true;
	}

	//bool DeSerialize(tesseract::TFile* fp) {
	//	return DeSerializeSize(fp) &&
	//		fp->DeSerialize(&empty_) &&
	//		fp->DeSerialize(&array_[0], num_elements());
	//}

	// Writes to the given file. Returns false in case of error.
	// Assumes a T::Serialize(FILE*) const function.
	bool SerializeClasses(FILE* fp) const {
		if (!SerializeSize(fp)) return false;
		if (!empty_.Serialize(fp)) return false;
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			if (!array_[i].Serialize(fp)) return false;
		}
		return true;
	}

	// Reads from the given file. Returns false in case of error.
	// Assumes a T::DeSerialize(bool swap, FILE*) function.
	// If swap is true, assumes a big/little-endian swap is needed.
	bool DeSerializeClasses(bool swap, FILE* fp) {
		if (!DeSerializeSize(swap, fp)) return false;
		if (!empty_.DeSerialize(swap, fp)) return false;
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			if (!array_[i].DeSerialize(swap, fp)) return false;
		}
		return true;
	}

	// Provide the dimensions of this rectangular matrix.
	int dim1() const { return dim1_; }
	int dim2() const { return dim2_; }
	// Returns the number of elements in the array.
	// Banded/triangular matrices may override.
	virtual int num_elements() const { return dim1_ * dim2_; }

	// Expression to select a specific location in the matrix. The matrix is
	// stored COLUMN-major, so the left-most index is the most significant.
	// This allows [][] access to use indices in the same order as (,).
	virtual int index(int column, int row) const {
		return (column * dim2_ + row);
	}

	// Put a list element into the matrix at a specific location.
	void put(ICOORD pos, const T& thing) {
		array_[this->index(pos.x(), pos.y())] = thing;
	}
	void put(int column, int row, const T& thing) {
		array_[this->index(column, row)] = thing;
	}

	// Get the item at a specified location from the matrix.
	T get(ICOORD pos) const {
		return array_[this->index(pos.x(), pos.y())];
	}
	T get(int column, int row) const {
		return array_[this->index(column, row)];
	}
	// Return a reference to the element at the specified location.
	const T& operator()(int column, int row) const {
		return array_[this->index(column, row)];
	}
	T& operator()(int column, int row) {
		return array_[this->index(column, row)];
	}
	// Allow access using array[column][row]. NOTE that the indices are
	// in the same left-to-right order as the () indexing.
	T* operator[](int column) {
		return &array_[this->index(column, 0)];
	}
	const T* operator[](int column) const {
		return &array_[this->index(column, 0)];
	}

	// Adds addend to *this, element-by-element.
	void operator+=(const GENERIC_2D_ARRAY<T>& addend) {
		if (dim2_ == addend.dim2_) {
			// Faster if equal size in the major dimension.
			int size = std::min(num_elements(), addend.num_elements());
			for (int i = 0; i < size; ++i) {
				array_[i] += addend.array_[i];
			}
		}
		else {
			for (int x = 0; x < dim1_; x++) {
				for (int y = 0; y < dim2_; y++) {
					(*this)(x, y) += addend(x, y);
				}
			}
		}
	}
	// Subtracts minuend from *this, element-by-element.
	void operator-=(const GENERIC_2D_ARRAY<T>& minuend) {
		if (dim2_ == minuend.dim2_) {
			// Faster if equal size in the major dimension.
			int size = std::min(num_elements(), minuend.num_elements());
			for (int i = 0; i < size; ++i) {
				array_[i] -= minuend.array_[i];
			}
		}
		else {
			for (int x = 0; x < dim1_; x++) {
				for (int y = 0; y < dim2_; y++) {
					(*this)(x, y) -= minuend(x, y);
				}
			}
		}
	}
	// Adds addend to all elements.
	void operator+=(const T& addend) {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			array_[i] += addend;
		}
	}
	// Multiplies *this by factor, element-by-element.
	void operator*=(const T& factor) {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			array_[i] *= factor;
		}
	}
	// Clips *this to the given range.
	void Clip(const T& rangemin, const T& rangemax) {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			array_[i] = ClipToRange(array_[i], rangemin, rangemax);
		}
	}
	// Returns true if all elements of *this are within the given range.
	// Only uses operator<
	bool WithinBounds(const T& rangemin, const T& rangemax) const {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			const T& value = array_[i];
			if (value < rangemin || rangemax < value)
				return false;
		}
		return true;
	}
	// Normalize the whole array.
	double Normalize() {
		int size = num_elements();
		if (size <= 0) return 0.0;
		// Compute the mean.
		double mean = 0.0;
		for (int i = 0; i < size; ++i) {
			mean += array_[i];
		}
		mean /= size;
		// Subtract the mean and compute the standard deviation.
		double sd = 0.0;
		for (int i = 0; i < size; ++i) {
			double normed = array_[i] - mean;
			array_[i] = normed;
			sd += normed * normed;
		}
		sd = sqrt(sd / size);
		if (sd > 0.0) {
			// Divide by the sd.
			for (int i = 0; i < size; ++i) {
				array_[i] /= sd;
			}
		}
		return sd;
	}

	// Returns the maximum value of the array.
	T Max() const {
		int size = num_elements();
		if (size <= 0) return empty_;
		// Compute the max.
		T max_value = array_[0];
		for (int i = 1; i < size; ++i) {
			const T& value = array_[i];
			if (value > max_value) max_value = value;
		}
		return max_value;
	}

	// Returns the maximum absolute value of the array.
	T MaxAbs() const {
		int size = num_elements();
		if (size <= 0) return empty_;
		// Compute the max.
		T max_abs = static_cast<T>(0);
		for (int i = 0; i < size; ++i) {
			T value = static_cast<T>(fabs(array_[i]));
			if (value > max_abs) max_abs = value;
		}
		return max_abs;
	}

	// Accumulates the element-wise sums of squares of src into *this.
	void SumSquares(const GENERIC_2D_ARRAY<T>& src, const T& decay_factor) {
		T update_factor = 1.0 - decay_factor;
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			array_[i] = array_[i] * decay_factor +
				update_factor * src.array_[i] * src.array_[i];
		}
	}

	// Scales each element using the adam algorithm, ie array_[i] by
	// sqrt(sqsum[i] + epsilon)).
	void AdamUpdate(const GENERIC_2D_ARRAY<T>& sum,
		const GENERIC_2D_ARRAY<T>& sqsum, const T& epsilon) {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			array_[i] += sum.array_[i] / (sqrt(sqsum.array_[i]) + epsilon);
		}
	}

	void AssertFinite() const {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			ASSERT_HOST(isfinite(array_[i]));
		}
	}

	// REGARDLESS OF THE CURRENT DIMENSIONS, treats the data as a
	// num_dims-dimensional array/tensor with dimensions given by dims, (ordered
	// from most significant to least significant, the same as standard C arrays)
	// and moves src_dim to dest_dim, with the initial dest_dim and any dimensions
	// in between shifted towards the hole left by src_dim. Example:
	// Current data content: array_=[0, 1, 2, ....119]
	//   perhaps *this may be of dim[40, 3], with values [[0, 1, 2][3, 4, 5]...
	//   but the current dimensions are irrelevant.
	// num_dims = 4, dims=[5, 4, 3, 2]
	// src_dim=3, dest_dim=1
	// tensor=[[[[0, 1][2, 3][4, 5]]
	//          [[6, 7][8, 9][10, 11]]
	//          [[12, 13][14, 15][16, 17]]
	//          [[18, 19][20, 21][22, 23]]]
	//         [[[24, 25]...
	// output dims =[5, 2, 4, 3]
	// output tensor=[[[[0, 2, 4][6, 8, 10][12, 14, 16][18, 20, 22]]
	//                 [[1, 3, 5][7, 9, 11][13, 15, 17][19, 21, 23]]]
	//                [[[24, 26, 28]...
	// which is stored in the array_ as:
	//   [0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 1, 3, 5, 7, 9, 11, 13...]
	// NOTE: the 2 stored matrix dimensions are simply copied from *this. To
	// change the dimensions after the transpose, use ResizeNoInit.
	// Higher dimensions above 2 are strictly the responsibility of the caller.
	void RotatingTranspose(const int* dims, int num_dims, int src_dim,
		int dest_dim, GENERIC_2D_ARRAY<T>* result) const {
		int max_d = std::max(src_dim, dest_dim);
		int min_d = std::min(src_dim, dest_dim);
		// In a tensor of shape [d0, d1... min_d, ... max_d, ... dn-2, dn-1], the
		// ends outside of min_d and max_d are unaffected, with [max_d +1, dn-1]
		// being contiguous blocks of data that will move together, and
		// [d0, min_d -1] being replicas of the transpose operation.
		// num_replicas represents the large dimensions unchanged by the operation.
		// move_size represents the small dimensions unchanged by the operation.
		// src_step represents the stride in the src between each adjacent group
		// in the destination.
		int num_replicas = 1, move_size = 1, src_step = 1;
		for (int d = 0; d < min_d; ++d) num_replicas *= dims[d];
		for (int d = max_d + 1; d < num_dims; ++d) move_size *= dims[d];
		for (int d = src_dim + 1; d < num_dims; ++d) src_step *= dims[d];
		if (src_dim > dest_dim) src_step *= dims[src_dim];
		// wrap_size is the size of a single replica, being the amount that is
		// handled num_replicas times.
		int wrap_size = move_size;
		for (int d = min_d; d <= max_d; ++d) wrap_size *= dims[d];
		result->ResizeNoInit(dim1_, dim2_);
		result->empty_ = empty_;
		const T* src = array_;
		T* dest = result->array_;
		for (int replica = 0; replica < num_replicas; ++replica) {
			for (int start = 0; start < src_step; start += move_size) {
				for (int pos = start; pos < wrap_size; pos += src_step) {
					memcpy(dest, src + pos, sizeof(*dest) * move_size);
					dest += move_size;
				}
			}
			src += wrap_size;
		}
	}

	// Delete objects pointed to by array_[i].
	void delete_matrix_pointers() {
		int size = num_elements();
		for (int i = 0; i < size; ++i) {
			T matrix_cell = array_[i];
			if (matrix_cell != empty_)
				delete matrix_cell;
		}
	}

protected:
	// Factored helper to serialize the size.
	bool SerializeSize(FILE* fp) const {
		uint32_t size = dim1_;
		if (!tesseract::Serialize(fp, &size)) return false;
		size = dim2_;
		return tesseract::Serialize(fp, &size);
	}
	//bool SerializeSize(tesseract::TFile* fp) const {
	//	uint32_t size = dim1_;
	//	if (!fp->Serialize(&size)) return false;
	//	size = dim2_;
	//	return fp->Serialize(&size);
	//}
	// Factored helper to deserialize the size.
	// If swap is true, assumes a big/little-endian swap is needed.
	bool DeSerializeSize(bool swap, FILE* fp) {
		uint32_t size1, size2;
		if (!tesseract::DeSerialize(fp, &size1)) return false;
		if (!tesseract::DeSerialize(fp, &size2)) return false;
		if (swap) {
			ReverseN(&size1, sizeof(size1));
			ReverseN(&size2, sizeof(size2));
		}
		// Arbitrarily limit the number of elements to protect against bad data.
		if (size1 > UINT16_MAX) return false;
		if (size2 > UINT16_MAX) return false;
		Resize(size1, size2, empty_);
		return true;
	}
	//bool DeSerializeSize(tesseract::TFile* fp) {
	//	int32_t size1, size2;
	//	if (!fp->DeSerialize(&size1)) return false;
	//	if (!fp->DeSerialize(&size2)) return false;
	//	// Arbitrarily limit the number of elements to protect against bad data.
	//	if (size1 > UINT16_MAX) return false;
	//	if (size2 > UINT16_MAX) return false;
	//	Resize(size1, size2, empty_);
	//	return true;
	//}

	T* array_;
	T empty_;   // The unused cell.
	int dim1_;  // Size of the 1st dimension in indexing functions.
	int dim2_;  // Size of the 2nd dimension in indexing functions.
	// The total size to which the array can be expanded before a realloc is
	// needed. If Resize is used, memory is retained so it can be re-expanded
	// without a further alloc, and this stores the allocated size.
	int size_allocated_;
};
//--------------------------------

//---------------------------------
#define CROSS(a,b) \
((a).x * (b).y - (a).y * (b).x)

struct TPOINT {
	TPOINT() : x(0), y(0) {}
	TPOINT(int16_t vx, int16_t vy) : x(vx), y(vy) {}
	TPOINT(const ICOORD &ic) : x(ic.x()), y(ic.y()) {}

	void operator+=(const TPOINT& other) {
		x += other.x;
		y += other.y;
	}
	void operator/=(int divisor) {
		x /= divisor;
		y /= divisor;
	}
	bool operator==(const TPOINT& other) const {
		return x == other.x && y == other.y;
	}
	// Returns true when the two line segments cross each other.
	// (Moved from outlines.cpp).
	static bool IsCrossed(const TPOINT& a0, const TPOINT& a1, const TPOINT& b0,
		const TPOINT& b1) {
		int b0a1xb0b1, b0b1xb0a0;
		int a1b1xa1a0, a1a0xa1b0;

		TPOINT b0a1, b0a0, a1b1, b0b1, a1a0;

		b0a1.x = a1.x - b0.x;
		b0a0.x = a0.x - b0.x;
		a1b1.x = b1.x - a1.x;
		b0b1.x = b1.x - b0.x;
		a1a0.x = a0.x - a1.x;
		b0a1.y = a1.y - b0.y;
		b0a0.y = a0.y - b0.y;
		a1b1.y = b1.y - a1.y;
		b0b1.y = b1.y - b0.y;
		a1a0.y = a0.y - a1.y;

		b0a1xb0b1 = CROSS(b0a1, b0b1);
		b0b1xb0a0 = CROSS(b0b1, b0a0);
		a1b1xa1a0 = CROSS(a1b1, a1a0);
		// For clarity, we want CROSS(a1a0,a1b0) here but we have b0a1 instead of a1b0
		// so use -CROSS(a1b0,b0a1) instead, which is the same.
		a1a0xa1b0 = -CROSS(a1a0, b0a1);

		return ((b0a1xb0b1 > 0 && b0b1xb0a0 > 0) ||
			(b0a1xb0b1 < 0 && b0b1xb0a0 < 0)) &&
			((a1b1xa1a0 > 0 && a1a0xa1b0 > 0) || (a1b1xa1a0 < 0 && a1a0xa1b0 < 0));
	}

	int16_t x;                       // absolute x coord.
	int16_t y;                       // absolute y coord.
};
//---------------------------------

// Tolerance in pixels used for baseline and xheight on non-upper/lower scripts.
static const int kSloppyTolerance = 4;
// Final tolerance in pixels added to the computed xheight range.
static const float kFinalPixelTolerance = 0.125f;

static const int kBlnCellHeight = 256;     // Full-height for baseline normalization.
static const int kBlnXHeight = 128;        // x-height for baseline normalization.
static const int kBlnBaselineOffset = 64;  // offset for baseline normalization.

//----------------------------------------
// Helper for SetupNonLinear computes an image of shortest run-lengths from
// the x/y edges provided.
// Based on "A nonlinear normalization method for handprinted Kanji character
// recognition -- line density equalization" by Hiromitsu Yamada et al.
// Eg below is an O in a 1-pixel margin-ed bounding box and the corresponding
//  ______________     input x_coords and y_coords.
// |  _________  |     <empty>
// | |    _    | |     1, 6
// | |   | |   | |     1, 3, 4, 6
// | |   | |   | |     1, 3, 4, 6
// | |   | |   | |     1, 3, 4, 6
// | |   |_|   | |     1, 3, 4, 6
// | |_________| |     1, 6
// |_____________|     <empty>
//  E 1 1 1 1 1 E
//  m 7 7 2 7 7 m
//  p     6     p
//  t     7     t
//  y           y
// The output image contains the min of the x and y run-length (distance
// between edges) at each coordinate in the image thus:
//  ______________
// |7 1_1_1_1_1 7|
// |1|5 5 1 5 5|1|
// |1|2 2|1|2 2|1|
// |1|2 2|1|2 2|1|
// |1|2 2|1|2 2|1|
// |1|2 2|1|2 2|1|
// |1|5_5_1_5_5|1|
// |7_1_1_1_1_1_7|
// Note that the input coords are all integer, so all partial pixels are dealt
// with elsewhere. Although it is nice for outlines to be properly connected
// and continuous, there is no requirement that they be as such, so they could
// have been derived from a flaky source, such as greyscale.
// This function works only within the provided box, and it is assumed that the
// input x_coords and y_coords have already been translated to have the bottom-
// left of box as the origin. Although an output, the minruns should have been
// pre-initialized to be the same size as box. Each element will contain the
// minimum of x and y run-length as shown above.
static void ComputeRunlengthImage(
	const TBOX& box,
	const std::vector<std::vector<int> >& x_coords,
	const std::vector<std::vector<int> >& y_coords,
	GENERIC_2D_ARRAY<int>* minruns) {
	int width = box.width();
	int height = box.height();
	ASSERT_HOST(minruns->dim1() == width);
	ASSERT_HOST(minruns->dim2() == height);
	// Set a 2-d image array to the run lengths at each pixel.
	for (int ix = 0; ix < width; ++ix) {
		int y = 0;
		for (int i = 0; i < y_coords[ix].size(); ++i) {
			int y_edge = ClipToRange(y_coords[ix][i], 0, height);
			int gap = y_edge - y;
			// Every pixel between the last and current edge get set to the gap.
			while (y < y_edge) {
				(*minruns)(ix, y) = gap;
				++y;
			}
		}
		// Pretend there is a bounding box of edges all around the image.
		int gap = height - y;
		while (y < height) {
			(*minruns)(ix, y) = gap;
			++y;
		}
	}
	// Now set the image pixels the the MIN of the x and y runlengths.
	for (int iy = 0; iy < height; ++iy) {
		int x = 0;
		for (int i = 0; i < x_coords[iy].size(); ++i) {
			int x_edge = ClipToRange(x_coords[iy][i], 0, width);
			int gap = x_edge - x;
			while (x < x_edge) {
				if (gap < (*minruns)(x, iy))
					(*minruns)(x, iy) = gap;
				++x;
			}
		}
		int gap = width - x;
		while (x < width) {
			if (gap < (*minruns)(x, iy))
				(*minruns)(x, iy) = gap;
			++x;
		}
	}
}
// Converts the run-length image (see above to the edge density profiles used
// for scaling, thus:
//  ______________
// |7 1_1_1_1_1 7|  = 5.28
// |1|5 5 1 5 5|1|  = 3.8
// |1|2 2|1|2 2|1|  = 5
// |1|2 2|1|2 2|1|  = 5
// |1|2 2|1|2 2|1|  = 5
// |1|2 2|1|2 2|1|  = 5
// |1|5_5_1_5_5|1|  = 3.8
// |7_1_1_1_1_1_7|  = 5.28
//  6 4 4 8 4 4 6
//  . . . . . . .
//  2 4 4 0 4 4 2
//  8           8
// Each profile is the sum of the reciprocals of the pixels in the image in
// the appropriate row or column, and these are then normalized to sum to 1.
// On output hx, hy contain an extra element, which will eventually be used
// to guarantee that the top/right edge of the box (and anything beyond) always
// gets mapped to the maximum target coordinate.
static void ComputeEdgeDensityProfiles(const TBOX& box,
	const GENERIC_2D_ARRAY<int>& minruns,
	std::vector<float>* hx,
	std::vector<float>* hy) {
	int width = box.width();
	int height = box.height();
	hx->assign(width + 1, 0.0);
	hy->assign(height + 1, 0.0);
	double total = 0.0;
	for (int iy = 0; iy < height; ++iy) {
		for (int ix = 0; ix < width; ++ix) {
			int run = minruns(ix, iy);
			if (run == 0) run = 1;
			float density = 1.0f / run;
			(*hx)[ix] += density;
			(*hy)[iy] += density;
		}
		total += (*hy)[iy];
	}
	// Normalize each profile to sum to 1.
	if (total > 0.0) {
		for (int ix = 0; ix < width; ++ix) {
			(*hx)[ix] /= total;
		}
		for (int iy = 0; iy < height; ++iy) {
			(*hy)[iy] /= total;
		}
	}
	// There is an extra element in each array, so initialize to 1.
	(*hx)[width] = 1.0f;
	(*hy)[height] = 1.0f;
}
//----------------------------------------

/// @class DENORM
class DENORM {
public:
	DENORM() {
		Init();
	}

	// Copying a DENORM is allowed.
	DENORM(const DENORM &src) {
		rotation_ = nullptr;
		*this = src;
	}
	DENORM& operator=(const DENORM &src) {
		Clear();
		inverse_ = src.inverse_;
		predecessor_ = src.predecessor_;
	//	pix_ = src.pix_;
	//	block_ = src.block_;
		if (src.rotation_ == nullptr)
			rotation_ = nullptr;
		else
			rotation_ = new FCOORD(*src.rotation_);
		x_origin_ = src.x_origin_;
		y_origin_ = src.y_origin_;
		x_scale_ = src.x_scale_;
		y_scale_ = src.y_scale_;
		final_xshift_ = src.final_xshift_;
		final_yshift_ = src.final_yshift_;
		return *this;
	}

	~DENORM() {
		Clear();
	}

	// Setup the normalization transformation parameters.
	// The normalizations applied to a blob are as follows:
	// 1. An optional block layout rotation that was applied during layout
	// analysis to make the textlines horizontal.
	// 2. A normalization transformation (LocalNormTransform):
	// Subtract the "origin"
	// Apply an x,y scaling.
	// Apply an optional rotation.
	// Add back a final translation.
	// The origin is in the block-rotated space, and is usually something like
	// the x-middle of the word at the baseline.
	// 3. Zero or more further normalization transformations that are applied
	// in sequence, with a similar pattern to the first normalization transform.
	//
	// A DENORM holds the parameters of a single normalization, and can execute
	// both the LocalNormTransform (a forwards normalization), and the
	// LocalDenormTransform which is an inverse transform or de-normalization.
	// A DENORM may point to a predecessor DENORM, which is actually the earlier
	// normalization, so the full normalization sequence involves executing all
	// predecessors first and then the transform in "this".
	// Let x be image co-ordinates and that we have normalization classes A, B, C
	// where we first apply A then B then C to get normalized x':
	// x' = CBAx
	// Then the backwards (to original coordinates) would be:
	// x = A^-1 B^-1 C^-1 x'
	// and A = B->predecessor_ and B = C->predecessor_
	// NormTransform executes all predecessors recursively, and then this.
	// NormTransform would be used to transform an image-based feature to
	// normalized space for use in a classifier
	// DenormTransform inverts this and then all predecessors. It can be
	// used to get back to the original image coordinates from normalized space.
	// The LocalNormTransform member executes just the transformation
	// in "this" without the layout rotation or any predecessors. It would be
	// used to run each successive normalization, eg the word normalization,
	// and later the character normalization.

	// Arguments:
	// block: if not nullptr, then this is the first transformation, and
	//        block->re_rotation() needs to be used after the Denorm
	//        transformation to get back to the image coords.
	// rotation: if not nullptr, apply this rotation after translation to the
	//           origin and scaling. (Usually a classify rotation.)
	// predecessor: if not nullptr, then predecessor has been applied to the
	//              input space and needs to be undone to complete the inverse.
	// The above pointers are not owned by this DENORM and are assumed to live
	// longer than this denorm, except rotation, which is deep copied on input.
	//
	// x_origin: The x origin which will be mapped to final_xshift in the result.
	// y_origin: The y origin which will be mapped to final_yshift in the result.
	//           Added to result of row->baseline(x) if not nullptr.
	//
	// x_scale: scale factor for the x-coordinate.
	// y_scale: scale factor for the y-coordinate. Ignored if segs is given.
	// Note that these scale factors apply to the same x and y system as the
	// x-origin and y-origin apply, ie after any block rotation, but before
	// the rotation argument is applied.
	//
	// final_xshift: The x component of the final translation.
	// final_yshift: The y component of the final translation.
	//
	// In theory, any of the commonly used normalizations can be setup here:
	// * Traditional baseline normalization on a word:
	// SetupNormalization(block, nullptr, nullptr,
	//                    box.x_middle(), baseline,
	//                    kBlnXHeight / x_height, kBlnXHeight / x_height,
	//                    0, kBlnBaselineOffset);
	// * "Numeric mode" baseline normalization on a word, in which the blobs
	//   are positioned with the bottom as the baseline is achieved by making
	//   a separate DENORM for each blob.
	// SetupNormalization(block, nullptr, nullptr,
	//                    box.x_middle(), box.bottom(),
	//                    kBlnXHeight / x_height, kBlnXHeight / x_height,
	//                    0, kBlnBaselineOffset);
	// * Anisotropic character normalization used by IntFx.
	// SetupNormalization(nullptr, nullptr, denorm,
	//                    centroid_x, centroid_y,
	//                    51.2 / ry, 51.2 / rx, 128, 128);
	// * Normalize blob height to x-height (current OSD):
	// SetupNormalization(nullptr, &rotation, nullptr,
	//                    box.rotational_x_middle(rotation),
	//                    box.rotational_y_middle(rotation),
	//                    kBlnXHeight / box.rotational_height(rotation),
	//                    kBlnXHeight / box.rotational_height(rotation),
	//                    0, kBlnBaselineOffset);
	// * Secondary normalization for classification rotation (current):
	// FCOORD rotation = block->classify_rotation();
	// float target_height = kBlnXHeight / CCStruct::kXHeightCapRatio;
	// SetupNormalization(nullptr, &rotation, denorm,
	//                    box.rotational_x_middle(rotation),
	//                    box.rotational_y_middle(rotation),
	//                    target_height / box.rotational_height(rotation),
	//                    target_height / box.rotational_height(rotation),
	//                    0, kBlnBaselineOffset);
	// * Proposed new normalizations for CJK: Between them there is then
	// no need for further normalization at all, and the character fills the cell.
	// ** Replacement for baseline normalization on a word:
	// Scales height and width independently so that modal height and pitch
	// fill the cell respectively.
	// float cap_height = x_height / CCStruct::kXHeightCapRatio;
	// SetupNormalization(block, nullptr, nullptr,
	//                    box.x_middle(), cap_height / 2.0f,
	//                    kBlnCellHeight / fixed_pitch,
	//                    kBlnCellHeight / cap_height,
	//                    0, 0);
	// ** Secondary normalization for classification (with rotation) (proposed):
	// Requires a simple translation to the center of the appropriate character
	// cell, no further scaling and a simple rotation (or nothing) about the
	// cell center.
	// FCOORD rotation = block->classify_rotation();
	// SetupNormalization(nullptr, &rotation, denorm,
	//                    fixed_pitch_cell_center,
	//                    0.0f,
	//                    1.0f,
	//                    1.0f,
	//                    0, 0);
	void SetupNormalization(/*const BLOCK* block,*/
		const FCOORD* rotation,
		const DENORM* predecessor,
		float x_origin, float y_origin,
		float x_scale, float y_scale,
		float final_xshift, float final_yshift) {
		Clear();
	//	block_ = block;
		if (rotation == nullptr)
			rotation_ = nullptr;
		else
			rotation_ = new FCOORD(*rotation);
		predecessor_ = predecessor;
		x_origin_ = x_origin;
		y_origin_ = y_origin;
		x_scale_ = x_scale;
		y_scale_ = y_scale;
		final_xshift_ = final_xshift;
		final_yshift_ = final_yshift;
	}

	// Sets up the DENORM to execute a non-linear transformation based on
	// preserving an even distribution of stroke edges. The transformation
	// operates only within the given box, scaling input coords within the box
	// non-linearly to a box of target_width by target_height, with all other
	// coords being clipped to the box edge. As with SetupNormalization above,
	// final_xshift and final_yshift are applied after scaling, and the bottom-
	// left of box is used as a pre-scaling origin.
	// x_coords is a collection of the x-coords of vertical edges for each
	// y-coord starting at box.bottom().
	// y_coords is a collection of the y-coords of horizontal edges for each
	// x-coord starting at box.left().
	// Eg x_coords[0] is a collection of the x-coords of edges at y=bottom.
	// Eg x_coords[1] is a collection of the x-coords of edges at y=bottom + 1.
	// The second-level vectors must all be sorted in ascending order.
	void SetupNonLinear(
		const DENORM* predecessor, const TBOX& box, float target_width,
		float target_height, float final_xshift, float final_yshift,
		const std::vector<std::vector<int> >& x_coords,
		const std::vector<std::vector<int> >& y_coords) {
		Clear();
		predecessor_ = predecessor;
		// x_map_ and y_map_ store a mapping from input x and y coordinate to output
		// x and y coordinate, based on scaling to the supplied target_width and
		// target_height.
		x_map_ = new std::vector<float>;
		y_map_ = new std::vector<float>;
		// Set a 2-d image array to the run lengths at each pixel.
		int width = box.width();
		int height = box.height();
		GENERIC_2D_ARRAY<int> minruns(width, height, 0);
		ComputeRunlengthImage(box, x_coords, y_coords, &minruns);
		// Edge density is the sum of the inverses of the run lengths. Compute
		// edge density projection profiles.
		ComputeEdgeDensityProfiles(box, minruns, x_map_, y_map_);
		// Convert the edge density profiles to the coordinates by multiplying by
		// the desired size and accumulating.
		(*x_map_)[width] = target_width;
		for (int x = width - 1; x >= 0; --x) {
			(*x_map_)[x] = (*x_map_)[x + 1] - (*x_map_)[x] * target_width;
		}
		(*y_map_)[height] = target_height;
		for (int y = height - 1; y >= 0; --y) {
			(*y_map_)[y] = (*y_map_)[y + 1] - (*y_map_)[y] * target_height;
		}
		x_origin_ = box.left();
		y_origin_ = box.bottom();
		final_xshift_ = final_xshift;
		final_yshift_ = final_yshift;
	}

	// Transforms the given coords one step forward to normalized space, without
	// using any block rotation or predecessor.
	void LocalNormTransform(const TPOINT& pt, TPOINT* transformed) const {
		FCOORD src_pt(pt.x, pt.y);
		FCOORD float_result;
		LocalNormTransform(src_pt, &float_result);
		transformed->x = IntCastRounded(float_result.x());
		transformed->y = IntCastRounded(float_result.y());
	}	
	void LocalNormTransform(const FCOORD& pt, FCOORD* transformed) const {
		FCOORD translated(pt.x() - x_origin_, pt.y() - y_origin_);
		if (x_map_ != nullptr && y_map_ != nullptr) {
			int x = ClipToRange<int>(IntCastRounded(translated.x()), 0, x_map_->size() - 1);
			translated.set_x((*x_map_)[x]);
			int y = ClipToRange<int>(IntCastRounded(translated.y()), 0, y_map_->size() - 1);
			translated.set_y((*y_map_)[y]);
		}
		else {
			translated.set_x(translated.x() * x_scale_);
			translated.set_y(translated.y() * y_scale_);
			if (rotation_ != nullptr)
				translated.rotate(*rotation_);
		}
		transformed->set_x(translated.x() + final_xshift_);
		transformed->set_y(translated.y() + final_yshift_);
	}

	// Transforms the given coords forward to normalized space using the
	// full transformation sequence defined by the block rotation, the
	// predecessors, deepest first, and finally this. If first_norm is not nullptr,
	// then the first and deepest transformation used is first_norm, ending
	// with this, and the block rotation will not be applied.
	void NormTransform(const DENORM* first_norm, const TPOINT& pt,
		TPOINT* transformed) const {
		FCOORD src_pt(pt.x, pt.y);
		FCOORD float_result;
		NormTransform(first_norm, src_pt, &float_result);
		transformed->x = IntCastRounded(float_result.x());
		transformed->y = IntCastRounded(float_result.y());
	}
	void NormTransform(const DENORM* first_norm, const FCOORD& pt,
		FCOORD* transformed) const {
		FCOORD src_pt(pt);
		if (first_norm != this) {
			if (predecessor_ != nullptr) {
				predecessor_->NormTransform(first_norm, pt, &src_pt);
			}
			//else if (block_ != nullptr) {
			//	FCOORD fwd_rotation(block_->re_rotation().x(),
			//		-block_->re_rotation().y());
			//	src_pt.rotate(fwd_rotation);
			//}
		}
		LocalNormTransform(src_pt, transformed);
	}

	// Transforms the given coords one step back to source space, without
	// using to any block rotation or predecessor.
	void LocalDenormTransform(const TPOINT& pt, TPOINT* original) const {
		FCOORD src_pt(pt.x, pt.y);
		FCOORD float_result;
		LocalDenormTransform(src_pt, &float_result);
		original->x = IntCastRounded(float_result.x());
		original->y = IntCastRounded(float_result.y());
	}
	void LocalDenormTransform(const FCOORD& pt, FCOORD* original) const {
		FCOORD rotated(pt.x() - final_xshift_, pt.y() - final_yshift_);
		if (x_map_ != nullptr && y_map_ != nullptr) {
			auto binary_search = [&](const std::vector<float> &data_, const float target) {
					int bottom = 0;
					int top = data_.size();
					while (top - bottom > 1) {
						int middle = (bottom + top) / 2;
						if (data_[middle] > target) {
							top = middle;
						}
						else {
							bottom = middle;
						}
					}
					return bottom;
			};
			//int x = x_map_->binary_search(rotated.x());
			int x = binary_search(*x_map_, rotated.x());			
			original->set_x(x + x_origin_);
			//int y = y_map_->binary_search(rotated.y());
			int y = binary_search(*y_map_, rotated.y());
			original->set_y(y + y_origin_);
		}
		else {
			if (rotation_ != nullptr) {
				FCOORD inverse_rotation(rotation_->x(), -rotation_->y());
				rotated.rotate(inverse_rotation);
			}
			original->set_x(rotated.x() / x_scale_ + x_origin_);
			float y_scale = y_scale_;
			original->set_y(rotated.y() / y_scale + y_origin_);
		}
	}

	// Transforms the given coords all the way back to source image space using
	// the full transformation sequence defined by this and its predecessors
	// recursively, shallowest first, and finally any block re_rotation.
	// If last_denorm is not nullptr, then the last transformation used will
	// be last_denorm, and the block re_rotation will never be executed.
	void DenormTransform(const DENORM* last_denorm, const TPOINT& pt,
		TPOINT* original) const {
		FCOORD src_pt(pt.x, pt.y);
		FCOORD float_result;
		DenormTransform(last_denorm, src_pt, &float_result);
		original->x = IntCastRounded(float_result.x());
		original->y = IntCastRounded(float_result.y());
	}
	void DenormTransform(const DENORM* last_denorm, const FCOORD& pt,
		FCOORD* original) const {
		LocalDenormTransform(pt, original);
		if (last_denorm != this) {
			if (predecessor_ != nullptr) {
				predecessor_->DenormTransform(last_denorm, *original, original);
			}
			//else if (block_ != nullptr) {
			//	original->rotate(block_->re_rotation());
			//}
		}
	}

	// Normalize a blob using blob transformations. Less accurate, but
	// more accurately copies the old way.
	// NOT-USE for locketk9
	//void LocalNormBlob(TBLOB* blob) const {
	//	ICOORD translation(-IntCastRounded(x_origin_), -IntCastRounded(y_origin_));
	//	blob->Move(translation);
	//	if (y_scale_ != 1.0f)
	//		blob->Scale(y_scale_);
	//	if (rotation_ != nullptr)
	//		blob->Rotate(*rotation_);
	//	translation.set_x(IntCastRounded(final_xshift_));
	//	translation.set_y(IntCastRounded(final_yshift_));
	//	blob->Move(translation);
	//}

	// Fills in the x-height range accepted by the given unichar_id in blob
	// coordinates, given its bounding box in the usual baseline-normalized
	// coordinates, with some initial crude x-height estimate (such as word
	// size) and this denoting the transformation that was used.
	// Also returns the amount the character must have shifted up or down.
	// NOT-USE for locketk9
	//void XHeightRange(int unichar_id, const UNICHARSET& unicharset,
	//	const TBOX& bbox,
	//	float* min_xht, float* max_xht, float* yshift) const {
	//	// Default return -- accept anything.
	//	*yshift = 0.0f;
	//	*min_xht = 0.0f;
	//	*max_xht = FLT_MAX;

	//	if (!unicharset.top_bottom_useful())
	//		return;

	//	// Clip the top and bottom to the limit of normalized feature space.
	//	int top = ClipToRange<int>(bbox.top(), 0, kBlnCellHeight - 1);
	//	int bottom = ClipToRange<int>(bbox.bottom(), 0, kBlnCellHeight - 1);
	//	// A tolerance of yscale corresponds to 1 pixel in the image.
	//	double tolerance = y_scale();
	//	// If the script doesn't have upper and lower-case characters, widen the
	//	// tolerance to allow sloppy baseline/x-height estimates.
	//	if (!unicharset.script_has_upper_lower())
	//		tolerance = y_scale() * kSloppyTolerance;

	//	int min_bottom, max_bottom, min_top, max_top;
	//	unicharset.get_top_bottom(unichar_id, &min_bottom, &max_bottom,
	//		&min_top, &max_top);

	//	// Calculate the scale factor we'll use to get to image y-pixels
	//	double midx = (bbox.left() + bbox.right()) / 2.0;
	//	double ydiff = (bbox.top() - bbox.bottom()) + 2.0;
	//	FCOORD mid_bot(midx, bbox.bottom()), tmid_bot;
	//	FCOORD mid_high(midx, bbox.bottom() + ydiff), tmid_high;
	//	DenormTransform(nullptr, mid_bot, &tmid_bot);
	//	DenormTransform(nullptr, mid_high, &tmid_high);

	//	// bln_y_measure * yscale = image_y_measure
	//	double yscale = tmid_high.pt_to_pt_dist(tmid_bot) / ydiff;

	//	// Calculate y-shift
	//	int bln_yshift = 0, bottom_shift = 0, top_shift = 0;
	//	if (bottom < min_bottom - tolerance) {
	//		bottom_shift = bottom - min_bottom;
	//	}
	//	else if (bottom > max_bottom + tolerance) {
	//		bottom_shift = bottom - max_bottom;
	//	}
	//	if (top < min_top - tolerance) {
	//		top_shift = top - min_top;
	//	}
	//	else if (top > max_top + tolerance) {
	//		top_shift = top - max_top;
	//	}
	//	if ((top_shift >= 0 && bottom_shift > 0) ||
	//		(top_shift < 0 && bottom_shift < 0)) {
	//		bln_yshift = (top_shift + bottom_shift) / 2;
	//	}
	//	*yshift = bln_yshift * yscale;

	//	// To help very high cap/xheight ratio fonts accept the correct x-height,
	//	// and to allow the large caps in small caps to accept the xheight of the
	//	// small caps, add kBlnBaselineOffset to chars with a maximum max, and have
	//	// a top already at a significantly high position.
	//	if (max_top == kBlnCellHeight - 1 &&
	//		top > kBlnCellHeight - kBlnBaselineOffset / 2)
	//		max_top += kBlnBaselineOffset;
	//	top -= bln_yshift;
	//	int height = top - kBlnBaselineOffset;
	//	double min_height = min_top - kBlnBaselineOffset - tolerance;
	//	double max_height = max_top - kBlnBaselineOffset + tolerance;

	//	// We shouldn't try calculations if the characters are very short (for example
	//	// for punctuation).
	//	if (min_height > kBlnXHeight / 8 && height > 0) {
	//		float result = height * kBlnXHeight * yscale / min_height;
	//		*max_xht = result + kFinalPixelTolerance;
	//		result = height * kBlnXHeight * yscale / max_height;
	//		*min_xht = result - kFinalPixelTolerance;
	//	}
	//}

	// Prints the content of the DENORM for debug purposes.
	// NOT-USE for locketk9
	//void Print() const {
	//	if (pix_ != nullptr) {
	//		tprintf("Pix dimensions %d x %d x %d\n",
	//			pixGetWidth(pix_), pixGetHeight(pix_), pixGetDepth(pix_));
	//	}
	//	if (inverse_)
	//		tprintf("Inverse\n");
	//	//if (block_ && block_->re_rotation().x() != 1.0f) {
	//	//	tprintf("Block rotation %g, %g\n",
	//	//		block_->re_rotation().x(), block_->re_rotation().y());
	//	//}
	//	tprintf("Input Origin = (%g, %g)\n", x_origin_, y_origin_);
	//	if (x_map_ != nullptr && y_map_ != nullptr) {
	//		tprintf("x map:\n");
	//		for (int x = 0; x < x_map_->size(); ++x) {
	//			tprintf("%g ", (*x_map_)[x]);
	//		}
	//		tprintf("\ny map:\n");
	//		for (int y = 0; y < y_map_->size(); ++y) {
	//			tprintf("%g ", (*y_map_)[y]);
	//		}
	//		tprintf("\n");
	//	}
	//	else {
	//		tprintf("Scale = (%g, %g)\n", x_scale_, y_scale_);
	//		if (rotation_ != nullptr)
	//			tprintf("Rotation = (%g, %g)\n", rotation_->x(), rotation_->y());
	//	}
	//	tprintf("Final Origin = (%g, %g)\n", final_xshift_, final_xshift_);
	//	if (predecessor_ != nullptr) {
	//		tprintf("Predecessor:\n");
	//		predecessor_->Print();
	//	}
	//}

	//Pix* pix() const {
	//	return pix_;
	//}
	//void set_pix(Pix* pix) {
	//	pix_ = pix;
	//}
	bool inverse() const {
		return inverse_;
	}
	void set_inverse(bool value) {
		inverse_ = value;
	}
	const DENORM* RootDenorm() const {
		if (predecessor_ != nullptr)
			return predecessor_->RootDenorm();
		return this;
	}
	const DENORM* predecessor() const {
		return predecessor_;
	}
	// Accessors - perhaps should not be needed.
	float x_scale() const {
		return x_scale_;
	}
	float y_scale() const {
		return y_scale_;
	}
	//const BLOCK* block() const {
	//	return block_;
	//}
	//void set_block(const BLOCK* block) {
	//	block_ = block;
	//}

private:
	// Free allocated memory and clear pointers.
	void Clear() {
		delete x_map_;
		x_map_ = nullptr;
		delete y_map_;
		y_map_ = nullptr;
		delete rotation_;
		rotation_ = nullptr;
	}
	// Setup default values.
	void Init() {
		inverse_ = false;
	//	pix_ = nullptr;
	//	block_ = nullptr;
		rotation_ = nullptr;
		predecessor_ = nullptr;
		x_map_ = nullptr;
		y_map_ = nullptr;
		x_origin_ = 0.0f;
		y_origin_ = 0.0f;
		x_scale_ = 1.0f;
		y_scale_ = 1.0f;
		final_xshift_ = 0.0f;
		final_yshift_ = static_cast<float>(kBlnBaselineOffset);
	}

	// Best available image.
//	Pix* pix_;
	// True if the source image is white-on-black.
	bool inverse_;
	// Block the word came from. If not null, block->re_rotation() takes the
	// "untransformed" coordinates even further back to the original image.
	// Used only on the first DENORM in a chain.
//	const BLOCK* block_;	// NOT-USE for locketk9
	// Rotation to apply between translation to the origin and scaling.
	const FCOORD* rotation_;
	// Previous transformation in a chain.
	const DENORM* predecessor_;
	// Non-linear transformation maps directly from each integer offset from the
	// origin to the corresponding x-coord. Owned by the DENORM.
	std::vector<float>* x_map_;
	// Non-linear transformation maps directly from each integer offset from the
	// origin to the corresponding y-coord. Owned by the DENORM.
	std::vector<float>* y_map_;
	// x-coordinate to be mapped to final_xshift_ in the result.
	float x_origin_;
	// y-coordinate to be mapped to final_yshift_ in the result.
	float y_origin_;
	// Scale factors for x and y coords. Applied to pre-rotation system.
	float x_scale_;
	float y_scale_;
	// Destination coords of the x_origin_ and y_origin_.
	float final_xshift_;
	float final_yshift_;
};
