#ifndef MBUFFER_H_INCLUDED
#define MBUFFER_H_INCLUDED


// permanent RAM buffers 
//#define AUDIO_HEAP_SIZE	235520
#define AUDIO_HEAP_SIZE	115360
#define MUSIC_DATA_SIZE	100000


// music labels 

extern char _sbank1SegmentRomStart[];
extern char _sbank1SegmentRomEnd[];

extern char _wbank1SegmentRomStart[];
extern char _wbank1SegmentRomEnd[];
extern char _wbank2SegmentRomStart[];
extern char _wbank2SegmentRomEnd[];

extern char _pbank1SegmentRomStart[];
extern char _pbank1SegmentRomEnd[];
extern char _pbank2SegmentRomStart[];
extern char _pbank2SegmentRomEnd[];


extern char _mBank1SegmentRomStart[];
extern char _mBank1SegmentRomEnd[];
extern char _mBank2SegmentRomStart[];
extern char _mBank2SegmentRomEnd[];
extern char _mBank3SegmentRomStart[];
extern char _mBank3SegmentRomEnd[];
extern char _mBank4SegmentRomStart[];
extern char _mBank4SegmentRomEnd[];
extern char _mBank5SegmentRomStart[];
extern char _mBank5SegmentRomEnd[];
extern char _mBank6SegmentRomStart[];
extern char _mBank6SegmentRomEnd[];
extern char _mBank7SegmentRomStart[];
extern char _mBank7SegmentRomEnd[];


// 'dream' generate labels 
extern char  BaseNote [];
extern float Detune   [];
extern char  *fxs     [];
extern int   Priority [];


#define SBANK_START _sbank1SegmentRomStart
#define SBANK_END _sbank1SegmentRomEnd

#define WBANK_START	_wbank1SegmentRomStart
#define PBANK_START _pbank1SegmentRomStart
#define PBANK_END	_pbank1SegmentRomEnd

#define WBANK2_START _wbank2SegmentRomStart
#define PBANK2_START _pbank2SegmentRomStart
#define PBANK2_END	 _pbank2SegmentRomEnd


extern unsigned char	audio_memory[];
extern musConfig musicPlayer;


#endif