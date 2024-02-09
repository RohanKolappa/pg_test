// ============================================================================

#include <iostream>

// ============================================================================

#ifndef _Buffer_H_
#define _Buffer_H_

// ============================================================================

class Buffer{

public :

	Buffer();
	Buffer( const Buffer& );
	Buffer( const char *, const int l = -1 );
	~Buffer();

	Buffer& operator = ( const Buffer& );

	bool operator == ( const Buffer& ) const;
	bool operator != ( const Buffer& ) const;

	const char *getBase() const { return base; }
	int length() const { return used; }
	bool isEmpty() const { return ( used == 0 ); }

	bool contains( const char c ) const;

	Buffer& operator += ( const Buffer& );
	Buffer& operator += ( const char c );

	Buffer operator + ( const Buffer& ) const;
	Buffer operator + ( const char c ) const;

	char operator [] ( const int pos ) const { return base[pos]; }
	Buffer operator () ( const int start ) const;
	Buffer operator () ( const int start, const int len ) const;

	void set( const int pos, const char c ) { base[pos] = c; }

	friend std::ostream& operator << ( std::ostream&, const Buffer& );

	friend Buffer operator + ( const char *, const Buffer& );

protected :

private :

	// internal state...

	char	*base;
	int	used;
	int	memo;

};

// ============================================================================

#endif // _Buffer_H_

// ============================================================================

