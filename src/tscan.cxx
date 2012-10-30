/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2012
 
  This file is part of tscan

  tscan is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  tscan is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      
  or send mail to:
      
*/

#include <csignal>
#include <cerrno>
#include <string>
#include <algorithm>
#include <cstdio> // for remove()
#include "config.h"
#include "timblserver/TimblServerAPI.h"
#include "libfolia/folia.h"
#include "libfolia/document.h"
#include "ticcutils/StringOps.h"
#include "ticcutils/LogStream.h"
#include "tscan/Configuration.h"
#include "tscan/Alpino.h"
#include "tscan/decomp.h"

using namespace std;
using namespace TiCC;
using namespace folia;

TiCC::LogStream cur_log( "T-scan", StampMessage );

#define LOG (*Log(cur_log))
#define DBG (*Dbg(cur_log))

const double NA = -123456789;

string configFile = "tscan.cfg";
Configuration config;

struct cf_data {
  long int count;
  double freq;
};

struct settingData {
  void init( const Configuration& );
  bool doAlpino;
  bool doDecompound;
  string decompounderPath;
  string style;
  int rarityLevel;
  double polarity_threshold;
  map <string, string> adj_sem;
  map <string, string> noun_sem;
  map <string, string> verb_sem;
  map <string, double> pol_lex;
  map<string, cf_data> freq_lex;
};

settingData settings;

bool fill( map<string,string>& m, istream& is ){
  string line;
  while( getline( is, line ) ){
    vector<string> parts;
    int n = split_at( line, parts, "\t" ); // split at tab
    if ( n != 2 ){
      cerr << "skip line: " << line << " (expected 2 values, got " 
	   << n << ")" << endl;
      continue;
    }
    vector<string> vals;
    n = split_at( parts[1], vals, "," ); // split at ,
    if ( n == 1 ){
      m[parts[0]] = vals[0];
    }
    else if ( n == 0 ){
      cerr << "skip line: " << line << " (expected some values, got none."
	   << endl;
      continue;
    }
    else {
      map<string,int> stats;
      int max = 0;
      string topval;
      for ( size_t i=0; i< vals.size(); ++i ){
	if ( ++stats[vals[i]] > max ){
	  max = stats[vals[i]];
	  topval = vals[i];
	}
      }
      m[parts[0]] = topval;
    }
  }
  return true;
}

bool fill( map<string,string>& m, const string& filename ){
  ifstream is( filename.c_str() );
  if ( is ){
    return fill( m, is );
  }
  else {
    cerr << "couldn't open file: " << filename << endl;
  }
  return false;
}

bool fill( map<string,double>& m, istream& is ){
  string line;
  while( getline( is, line ) ){
    vector<string> parts;
    size_t n = split_at( line, parts, "\t" ); // split at tab
    if ( n != 2 ){
      cerr << "skip line: " << line << " (expected 2 values, got " 
	   << n << ")" << endl;
      continue;
    }
    double value = stringTo<double>( parts[1] );
    if ( abs(value) < settings.polarity_threshold ){
      value = 0;
    }
    // parts[0] is something like "sympathiek a"
    // is't not quite clear if there is exactly 1 space
    // also there is "dolce far niente n"
    // so we split again, and reconstruct using a colon before the pos.
    vector<string> vals;
    n = split_at( parts[0], vals, " " ); // split at space(s)
    if ( n < 2 ){
      cerr << "skip line: " << line << " (expected at least 2 values, got " 
	   << n << ")" << endl;
      continue;
    }
    string key;
    for ( size_t i=0; i < n-1; ++i )
      key += vals[i];
    key += ":" + vals[n-1];
    m[key] = value;
  }
  return true;
}

bool fill( map<string,double>& m, const string& filename ){
  ifstream is( filename.c_str() );
  if ( is ){
    return fill( m, is );
  }
  else {
    cerr << "couldn't open file: " << filename << endl;
  }
  return false;
}

bool fill( map<string,cf_data>& m, istream& is ){
  string line;
  while( getline( is, line ) ){
    vector<string> parts;
    size_t n = split_at( line, parts, "\t" ); // split at tabs
    if ( n != 4 ){
      cerr << "skip line: " << line << " (expected 4 values, got " 
	   << n << ")" << endl;
      continue;
    }
    cf_data data;
    data.count = stringTo<long int>( parts[1] );
    data.freq = stringTo<double>( parts[3] );
    m[parts[0]] = data;
  }
  return true;
}

bool fill( map<string,cf_data>& m, const string& filename ){
  ifstream is( filename.c_str() );
  if ( is ){
    return fill( m, is );
  }
  else {
    cerr << "couldn't open file: " << filename << endl;
  }
  return false;
}

void settingData::init( const Configuration& cf ){
  string val = cf.lookUp( "useAlpino" );
  doAlpino = false;
  if( !Timbl::stringTo( val, doAlpino ) ){
    cerr << "invalid value for 'useAlpino' in config file" << endl;
  }
  doDecompound = false;
  val = cf.lookUp( "decompounderPath" );
  if( !val.empty() ){
    decompounderPath = val + "/";
    doDecompound = true;
  }
  val = cf.lookUp( "styleSheet" );
  if( !val.empty() ){
    style = val;
  }
  val = cf.lookUp( "rarityLevel" );
  if ( val.empty() ){
    rarityLevel = 5;
  }
  else if ( !Timbl::stringTo( val, rarityLevel ) ){ 
    cerr << "invalid value for 'rarityLevel' in config file" << endl;
  }
  val = cf.lookUp( "adj_semtypes" );
  if ( !val.empty() ){
    if ( !fill( adj_sem, cf.configDir() + "/" + val ) )
      exit( EXIT_FAILURE );
  }
  val = cf.lookUp( "noun_semtypes" );
  if ( !val.empty() ){
    if ( !fill( noun_sem, cf.configDir() + "/" + val ) )
      exit( EXIT_FAILURE );
  }
  val = cf.lookUp( "verb_semtypes" );
  if ( !val.empty() ){
    if ( !fill( verb_sem, cf.configDir() + "/" + val ) )
      exit( EXIT_FAILURE );
  }
  val = cf.lookUp( "polarity_threshold" );
  if ( val.empty() ){
    polarity_threshold = 0.01;
  }
  else if ( !Timbl::stringTo( val, polarity_threshold ) ){ 
    cerr << "invalid value for 'polarity_threshold' in config file" << endl;
  }
  val = cf.lookUp( "polarity_lex" );
  if ( !val.empty() ){
    if ( !fill( pol_lex, cf.configDir() + "/" + val ) )
      exit( EXIT_FAILURE );
  }
  val = cf.lookUp( "freq_lex" );
  if ( !val.empty() ){
    if ( !fill( freq_lex, cf.configDir() + "/" + val ) )
      exit( EXIT_FAILURE );
  }
}

inline void usage(){
  cerr << "usage:  tscan" << endl;
}

template <class M>
void aggregate( M& out, const M& in ){
  typename M::const_iterator ii = in.begin();
  while ( ii != in.end() ){
    typename M::iterator oi = out.find( ii->first );
    if ( oi == out.end() ){
      out[ii->first] = ii->second;
    }
    else {
      oi->second += ii->second;
    }
    ++ii;
  }
}

enum WordProp { ISNAME, ISPUNCT, 
		ISVD, ISOD, ISINF, ISPVTGW, ISPVVERL,
		ISPPRON1, ISPPRON2, ISPPRON3,
		JUSTAWORD };

enum SemType { UNFOUND, CONCRETE, CONCRETE_HUMAN, ABSTRACT, BROAD, 
	       STATE, ACTION, PROCESS, WEIRD };

struct basicStats {
  basicStats( const string& cat ): category( cat ), 
				   wordLen(0),wordLenExNames(0),
				   morphLen(0), morphLenExNames(0) {};
  virtual ~basicStats(){};
  virtual void print( ostream& ) const = 0;
  virtual void addMetrics( FoliaElement *el ) const = 0;
  string category;
  int wordLen;
  int wordLenExNames;
  int morphLen;
  int morphLenExNames;
};

ostream& operator<<( ostream& os, const basicStats& b ){
  b.print(os);
  return os;
}

void basicStats::print( ostream& os ) const {
  os << "BASICSTATS PRINT" << endl;
  os << category << endl;
  os << " lengte=" << wordLen << ", aantal morphemen=" << morphLen << endl;
  os << " lengte zonder namem =" << wordLenExNames << ", aantal morphemen zonder namen=" << morphLenExNames << endl;
}

struct wordStats : public basicStats {
  wordStats( Word *, xmlDoc * );
  void print( ostream& ) const;
  void addMetrics( FoliaElement * ) const;
  bool checkContent() const;
  bool checkNominal( Word *, xmlDoc * ) const;
  WordProp checkProps( const PosAnnotation* );
  double checkPolarity( ) const;
  SemType checkSemProps( ) const;
  bool checkPropNeg() const;
  bool checkMorphNeg() const;
  void freqLookup();
  string word;
  string pos;
  string posHead;
  string lemma;
  string wwform;
  bool isPassive;
  bool isPronRef;
  bool archaic;
  bool isContent;
  bool isNominal;
  bool isOnder;
  bool isBetr;
  bool isPropNeg;
  bool isMorphNeg;
  bool f50;
  bool f65;
  bool f77;
  bool f80;
  int compLen;
  int wfreq;
  double lwfreq;
  double polarity;
  WordProp prop;
  SemType sem_type;
  vector<string> morphemes;
};

bool wordStats::checkContent() const {
  if ( posHead == "WW" ){
    if ( wwform == "hoofdww" ){
      return true;
    }
  }
  else {
    return ( posHead == "N" || posHead == "BW" || posHead == "ADJ" );
  }
  return false;
}

bool match_tail( const string& word, const string& tail ){
  if ( tail.size() > word.size() )
    return false;
  string::const_reverse_iterator wir = word.rbegin();
  string::const_reverse_iterator tir = tail.rbegin();
  while ( tir != tail.rend() ){
    if ( *tir != *wir )
      return false;
    ++tir;
    ++wir;
  }
  return false;
}

bool wordStats::checkNominal( Word *w, xmlDoc *alpDoc ) const {
  static string morphList[] = { "ing", "sel", "(e)nis", "heid", "te", "schap",
				"dom", "sie", "iek", "iteit", "age", "esse",
				"name" };
  static set<string> morphs( morphList, morphList + 13 );
  if ( posHead == "N" && morphemes.size() > 1 
       && morphs.find( morphemes[morphemes.size()-1] ) != morphs.end() ){
    return true;
  }
  bool matched = match_tail( word, "ose" ) ||
    match_tail( word, "ase" ) ||
    match_tail( word, "ese" ) ||
    match_tail( word, "isme" ) ||
    match_tail( word, "sie" ) ||
    match_tail( word, "tie" );
  if ( matched )
    return true;
  else {
    xmlNode *node = getAlpWord( alpDoc, w );
    if ( node ){
      KWargs args = getAttributes( node );
      if ( args["pos"] == "verb" ){
	node = node->parent;
	KWargs args = getAttributes( node );
	if ( args["cat"] == "np" )
	  return true;
      }
    }
  }
  return false;
}

WordProp wordStats::checkProps( const PosAnnotation* pa ) {
  if ( posHead == "LET" )
    prop = ISPUNCT;
  else if ( posHead == "SPEC" && pos.find("eigen") != string::npos )
    prop = ISNAME;
  else if ( posHead == "WW" ){
    string wvorm = pa->feat("wvorm");
    if ( wvorm == "inf" )
      prop = ISINF;
    else if ( wvorm == "vd" )
      prop = ISVD;
    else if ( wvorm == "od" )
      prop = ISOD;
    else if ( wvorm == "pv" ){
      string tijd = pa->feat("pvtijd");
      if ( tijd == "tgw" )
	prop = ISPVTGW;
      else if ( tijd == "verl" )
	prop = ISPVVERL;
      else {
	cerr << "PANIEK: een onverwachte ww tijd: " << tijd << endl;
	exit(3);
      }
    }
    else {
      cerr << "PANIEK: een onverwachte ww vorm: " << wvorm << endl;
      exit(3);
    }
  }
  else if ( posHead == "VNW" ){
    string vwtype = pa->feat("vwtype");
    isBetr = vwtype == "betr";
    if ( lowercase( word ) != "men" ){
      string cas = pa->feat("case");
      archaic = ( cas == "gen" || cas == "dat" );
      if ( vwtype == "pers" || vwtype == "refl" 
	   || vwtype == "pr" || vwtype == "bez" ) {
	string persoon = pa->feat("persoon");
	if ( !persoon.empty() ){
	  if ( persoon[0] == '1' )
	    prop = ISPPRON1;
	  else if ( persoon[0] == '2' )
	    prop = ISPPRON2;
	  else if ( persoon[0] == '3' ){
	    prop = ISPPRON3;
	    isPronRef = ( vwtype == "pers" || vwtype == "bez" );
	  }
	  else {
	    cerr << "PANIEK: een onverwachte PRONOUN persoon : " << persoon 
		 << " for word " << word << endl;
	    exit(3);
	  }
	}
      }
      else if ( vwtype == "aanw" )
	isPronRef = true;
    }
  }
  else if ( posHead == "LID" ) {
    string cas = pa->feat("case");
    archaic = ( cas == "gen" || cas == "dat" );
  }
  else if ( posHead == "VG" ) {
    string cp = pa->feat("conjtype");
    isOnder = cp == "onder";
  }
  return prop;
}

double wordStats::checkPolarity( ) const {
  string key = lowercase(word)+":";
  if ( posHead == "N" )
    key += "n";
  else if ( posHead == "ADJ" )
    key += "a";
  else if ( posHead == "WW" )
    key += "v";
  else
    return NA;
  map<string,double>::const_iterator it = settings.pol_lex.find( key );
  if ( it != settings.pol_lex.end() ){
    return it->second;
  }
  return NA;
}

SemType get_sem_type( const string& lemma, const string& pos ){
  if ( pos == "N" ){
    map<string,string>::const_iterator it = settings.noun_sem.find( lemma );
    if ( it != settings.noun_sem.end() ){
      string type = it->second;
      if ( type == "human" )
	return CONCRETE_HUMAN;
      else if ( type == "concrother" || type == "substance" 
		|| type == "artefact" || type == "nonhuman" )
	return CONCRETE;
      else if ( type == "dynamic" || type == "nondynamic" )
	return ABSTRACT;
      else 
	return BROAD;
    }
  }
  else if ( pos == "ADJ" ) {
    map<string,string>::const_iterator it = settings.adj_sem.find( lemma );
    if ( it != settings.adj_sem.end() ){
      string type = it->second;
      if ( type == "phyper" || type == "stuff" || type == "colour" )
	return CONCRETE;
      else if ( type == "abstract" )
	return ABSTRACT;
      else 
	return BROAD;
    }
  }
  else if ( pos == "WW" ) {
    map<string,string>::const_iterator it = settings.verb_sem.find( lemma );
    if ( it != settings.verb_sem.end() ){
      string type = it->second;
      if ( type == "state" )
	return STATE;
      else if ( type == "action" )
	return ACTION;
      else if ( type == "process" )
	return PROCESS;
      else
	return WEIRD;
    }
  }
  return UNFOUND;
}
  
SemType wordStats::checkSemProps( ) const {
  SemType type = get_sem_type( lemma, posHead );
  return type;
}

void wordStats::freqLookup(){
  map<string,cf_data>::const_iterator it = settings.freq_lex.find( lowercase(word) );
  if ( it != settings.freq_lex.end() ){
    wfreq = it->second.count;
    lwfreq = log(wfreq);
    double freq = it->second.freq;
    if ( freq <= 50 )
      f50 = true;
    if ( freq <= 65 )
      f65 = true;
    if ( freq <= 77 )
      f77 = true;
    if ( freq <= 80 )
      f80 = true;
  }
}

const string negativesA[] = { "geeneens", "geenszins", "kwijt", "nergens",
			      "niet", "niets", "nooit", "allerminst",
			      "allesbehalve", "amper", "behalve", "contra",
			      "evenmin", "geen", "generlei", "nauwelijks",
			      "niemand", "niemendal", "nihil", "niks", "nimmer",
			      "nimmermeer", "noch", "ongeacht", "slechts",
			      "tenzij", "ternauwernood", "uitgezonderd",
			      "weinig", "zelden", "zeldzaam", "zonder" };
set<string> negatives = set<string>( negativesA, negativesA + 32 );

bool wordStats::checkPropNeg() const {
  string lword = lowercase( word );
  if ( negatives.find( lword ) != negatives.end() ){
    return true;
  }
  else if ( posHead == "BW" &&
	    ( lword == "moeilijk" || lword == "weg" ) ){
    return true;
  }
  return false;
}

const string negmorphA[] = { "mis", "de", "non", "on" };
set<string> negmorphs = set<string>( negmorphA, negmorphA + 4 );

const string negminusA[] = { "mis-", "non-", "niet-", "anti-",
			     "ex-", "on-", "oud-" };
vector<string> negminus = vector<string>( negminusA, negminusA + 7 );

bool wordStats::checkMorphNeg() const {
  string m1 = morphemes[0];
  string m2;
  if ( morphemes.size() > 1 ){
    m2 = morphemes[1];
  }
  if ( negmorphs.find( m1 ) != negmorphs.end() && m2 != "en" && !m2.empty() ){
    return true;
  }
  else {
    string lword = lowercase( word );
    for ( size_t i=0; i < negminus.size(); ++i ){
      if ( word.find( negminus[i] ) != string::npos )
	return true;
    }
  }
  return false;
}

wordStats::wordStats( Word *w, xmlDoc *alpDoc ):
  basicStats( "WORD" ), 
  isPassive(false), isPronRef(false),
  archaic(false), isContent(false), isNominal(false),
  isOnder(false), isBetr(false), isPropNeg(false), isMorphNeg(false),
  f50(false), f65(false), f77(false), f80(false),  compLen(0), wfreq(0), lwfreq(0),
  polarity(NA), prop(JUSTAWORD), sem_type(UNFOUND)
{
  word = UnicodeToUTF8( w->text() );
  wordLen = w->text().length();
  vector<PosAnnotation*> posV = w->select<PosAnnotation>();
  if ( posV.size() != 1 )
    throw ValueError( "word doesn't have POS tag info" );
  PosAnnotation *pa = posV[0];
  pos = pa->cls();
  posHead = pa->feat("head");
  lemma = w->lemma();
  prop = checkProps( pa );
  if ( posHead == "WW" ){
    if ( alpDoc ){
      wwform = classifyVerb( w, alpDoc );
      isPassive = ( wwform == "passiefww" );
    }
  }
  isContent = checkContent();
  if ( prop != ISPUNCT ){
    size_t max = 0;
    vector<Morpheme*> morphemeV;
    vector<MorphologyLayer*> ml = w->annotations<MorphologyLayer>();
    for ( size_t q=0; q < ml.size(); ++q ){
      vector<Morpheme*> m = ml[q]->select<Morpheme>();
      if ( m.size() > max ){
	// a hack: we assume the longest morpheme list to 
	// be the best choice. 
	morphemeV = m;
	max = m.size();
      }
    }
    for ( size_t q=0; q < morphemeV.size(); ++q ){
      morphemes.push_back( morphemeV[q]->str() );
    }
    isPropNeg = checkPropNeg();
    isMorphNeg = checkMorphNeg();
    morphLen = max;
    if ( prop != ISNAME ){
      wordLenExNames = wordLen;
      morphLenExNames = max;
    }
    isNominal = checkNominal( w, alpDoc );
    polarity = checkPolarity();
    sem_type = checkSemProps();
    freqLookup();
    if ( settings.doDecompound )
      compLen = runDecompoundWord(word, settings.decompounderPath );
  }
  addMetrics( w );
}

void addOneMetric( Document *doc, FoliaElement *parent,
		   const string& cls, const string& val ){
  MetricAnnotation *m = new MetricAnnotation( doc,
					      "class='" + cls + "', value='" 
					      + val + "'" );
  parent->append( m );
}

void wordStats::addMetrics( FoliaElement *el ) const {
  if ( isContent )
    addOneMetric( el->doc(), el, 
		  "content_word", "true" );
  if ( archaic )
    addOneMetric( el->doc(), el, 
		  "archaic", "true" );
  if ( isNominal )
    addOneMetric( el->doc(), el, 
		  "nominalization", "true" );
  if ( isOnder )
    addOneMetric( el->doc(), el, 
		  "subordinate", "true" );
  if ( isBetr )
    addOneMetric( el->doc(), el, 
		  "betrekkelijk", "true" );
  if ( isPropNeg )
    addOneMetric( el->doc(), el, 
		  "proper_negative", "true" );
  if ( isMorphNeg )
    addOneMetric( el->doc(), el, 
		  "morph_negative", "true" );
  if ( polarity != NA  )
    addOneMetric( el->doc(), el, 
		  "polarity", toString(polarity) );
  if ( compLen > 0 )
    addOneMetric( el->doc(), el, 
		  "compound_len", toString(compLen) );

  addOneMetric( el->doc(), el, 
		"word_freq", toString(lwfreq) );
  if ( !wwform.empty() ){
    KWargs args;
    args["set"] = "tscan-set";
    args["class"] = "wwform(" + wwform + ")";
    el->addPosAnnotation( args );
  }
}

void wordStats::print( ostream& os ) const {
  os << "word: [" <<word << "], lengte=" << wordLen 
     << ", morphemen: " << morphemes << ", HEAD: " << posHead;
  switch ( prop ){
  case ISINF:
    os << " (Infinitief)";
    break;
  case ISVD:
    os << " (Voltooid deelwoord)";
    break;
  case ISOD:
    os << " (Onvoltooid Deelwoord)";
    break;
  case ISPVTGW:
    os << " (Tegenwoordige tijd)";
    break;
  case ISPVVERL:
    os << " (Verleden tijd)";
    break;
  case ISPPRON1:
    os << " (1-ste persoon)";
    break;
  case ISPPRON2:
    os << " (2-de persoon)";
    break;
  case ISPPRON3:
    os << " (3-de persoon)";
    break;
  case ISNAME:
    os << " (Name)";
    break;
  case ISPUNCT:
  case JUSTAWORD:
    break;
  }
  if ( !wwform.empty() )
    os << " (" << wwform << ")";  
  if ( isNominal )
    os << " nominalisatie";  
  if ( isOnder )
    os << " ondergeschikt";  
  if ( isBetr )
    os << " betrekkelijk";  
  if ( isPropNeg || isMorphNeg )
    os << " negatief";  
  if ( polarity != NA ){
    os << ", Polariteit=" << polarity << endl;
  }
}

enum NerProp { NONER, LOC_B, LOC_I, EVE_B, EVE_I, ORG_B, ORG_I, 
	       MISC_B, MISC_I, PER_B, PER_I, PRO_B, PRO_I };

ostream& operator<<( ostream& os, const NerProp& n ){
  switch ( n ){
  case NONER:
    break;
  case LOC_B:
  case LOC_I:
    os << "LOC";
    break;
  case EVE_B:
  case EVE_I:
    os << "EVE";
    break;
  case ORG_B:
  case ORG_I:
    os << "ORG";
    break;
  case MISC_B:
  case MISC_I:
    os << "MISC";
    break;
  case PER_B:
  case PER_I:
    os << "PER";
    break;
  case PRO_B:
  case PRO_I:
    os << "PRO";
    break;
  };
  return os;
}

struct structStats: public basicStats {
  structStats( const string& cat ): basicStats( cat ),
				    wordCnt(0),
				    vdCnt(0),odCnt(0),
				    infCnt(0), presentCnt(0), pastCnt(0),
				    nameCnt(0),
				    pron1Cnt(0), pron2Cnt(0), pron3Cnt(0), 
				    passiveCnt(0),
				    pronRefCnt(0), archaicsCnt(0),
				    contentCnt(0),
				    nominalCnt(0),
				    onderCnt(0),
				    betrCnt(0),
				    propNegCnt(0),
				    morphNegCnt(0),
				    f50Cnt(0),
				    f65Cnt(0),
				    f77Cnt(0),
				    f80Cnt(0),
				    compCnt(0),
				    compLen(0),
				    wfreq(0),
				    wfreq_n(0),
				    lwfreq(0),
				    lwfreq_n(0),
				    polarity(NA),
				    broadConcreteCnt(0),
				    strictConcreteCnt(0),
				    broadAbstractCnt(0),
				    strictAbstractCnt(0),
				    stateCnt(0),
				    actionCnt(0),
				    processCnt(0),
				    weirdCnt(0),
				    humanCnt(0),
				    npCnt(0),
				    npSize(0),
				    dLevel(-1)
 {};
  ~structStats();
  virtual void print(  ostream& ) const;
  void addMetrics( FoliaElement *el ) const;
  void merge( structStats * );
  string id;
  string text;
  int wordCnt;
  int vdCnt;
  int odCnt;
  int infCnt;
  int presentCnt;
  int pastCnt;
  int nameCnt;
  int pron1Cnt;
  int pron2Cnt;
  int pron3Cnt;
  int passiveCnt;
  int pronRefCnt;
  int archaicsCnt;
  int contentCnt;
  int nominalCnt;
  int onderCnt;
  int betrCnt;
  int propNegCnt;
  int morphNegCnt;
  int f50Cnt;
  int f65Cnt;
  int f77Cnt;
  int f80Cnt;
  int compCnt;
  int compLen;
  int wfreq; 
  int wfreq_n; 
  double lwfreq; 
  double lwfreq_n;
  double polarity;
  int broadConcreteCnt;
  int strictConcreteCnt;
  int broadAbstractCnt;
  int strictAbstractCnt;
  int stateCnt;
  int actionCnt;
  int processCnt;
  int weirdCnt;
  int humanCnt;
  int npCnt;
  int npSize;
  int dLevel;
  vector<basicStats *> sv;
  map<string,int> heads;
  map<string,int> unique_words;
  map<string,int> unique_lemmas;
  map<NerProp, int> ners;
};

structStats::~structStats(){
  vector<basicStats *>::iterator it = sv.begin();
  while ( it != sv.end() ){
    delete( *it );
    ++it;
  }
}

void structStats::merge( structStats *ss ){
  wordCnt += ss->wordCnt;
  wordLen += ss->wordLen;
  wordLenExNames += ss->wordLenExNames;
  morphLen += ss->morphLen;
  morphLenExNames += ss->morphLenExNames;
  nameCnt += ss->nameCnt;
  vdCnt += ss->vdCnt;
  odCnt += ss->odCnt;
  infCnt += ss->infCnt;
  passiveCnt += ss->passiveCnt;
  archaicsCnt += ss->archaicsCnt;
  contentCnt += ss->contentCnt;
  nominalCnt += ss->nominalCnt;
  onderCnt += ss->onderCnt;
  betrCnt += ss->betrCnt;
  propNegCnt += ss->propNegCnt;
  morphNegCnt += ss->morphNegCnt;
  f50Cnt += ss->f50Cnt;
  f65Cnt += ss->f65Cnt;
  f77Cnt += ss->f77Cnt;
  f80Cnt += ss->f80Cnt;
  compCnt += ss->compCnt;
  compLen += ss->compLen;
  wfreq += ss->wfreq;
  wfreq_n += ss->wfreq_n;
  if ( ss->polarity != NA ){
    if ( polarity == NA )
      polarity = ss->polarity;
    else
      polarity += ss->polarity;
  }
  presentCnt += ss->presentCnt;
  pastCnt += ss->pastCnt;
  pron1Cnt += ss->pron1Cnt;
  pron2Cnt += ss->pron2Cnt;
  pron3Cnt += ss->pron3Cnt;
  pronRefCnt += ss->pronRefCnt;
  strictAbstractCnt += ss->strictAbstractCnt;
  broadAbstractCnt += ss->broadAbstractCnt;
  strictConcreteCnt += ss->strictConcreteCnt;
  broadConcreteCnt += ss->broadConcreteCnt;
  stateCnt += ss->stateCnt;
  actionCnt += ss->actionCnt;
  processCnt += ss->processCnt;
  weirdCnt += ss->weirdCnt;
  humanCnt += ss->humanCnt;
  npCnt += ss->npCnt;
  npSize += ss->npSize;
  if ( ss->dLevel > 0 ){
    if ( dLevel < 0 )
      dLevel = ss->dLevel;
    else
      dLevel += ss->dLevel;
  }
  sv.push_back( ss );
  aggregate( heads, ss->heads );
  aggregate( unique_words, ss->unique_words );
  aggregate( unique_lemmas, ss->unique_lemmas );
  aggregate( ners, ss->ners );
}

void structStats::print( ostream& os ) const {
  os << category << " " << id << endl;
  os << category << " POS tags: ";
  for ( map<string,int>::const_iterator it = heads.begin();
	it != heads.end(); ++it ){
    os << it->first << "[" << it->second << "] ";
  }
  os << endl;
  os << "#Voltooid deelwoorden " << vdCnt 
     << " gemiddeld: " << vdCnt/double(wordCnt) << endl;
  os << "#Onvoltooid deelwoorden " << odCnt 
     << " gemiddeld: " << odCnt/double(wordCnt) << endl;
  os << "#Infinitieven " << infCnt 
     << " gemiddeld: " << infCnt/double(wordCnt) << endl;
  os << "#Archaische constructies " << archaicsCnt
     << " gemiddeld: " << archaicsCnt/double(wordCnt) << endl;
  os << "#Content woorden " << contentCnt
     << " gemiddeld: " << contentCnt/double(wordCnt) << endl;
  os << "#NP's " << npCnt << " met in totaal " << npSize
     << " woorden, gemiddeld: " << npSize/npCnt << endl;
  os << "D LEVEL=" << dLevel << endl;
  if ( polarity != NA ){
    os << "#Polariteit:" << polarity
       << " gemiddeld: " << polarity/double(wordCnt) << endl;
  }
  else {
    os << "#Polariteit: NA" << endl;
  }
  os << "#Nominalizaties " << nominalCnt
     << " gemiddeld: " << nominalCnt/double(wordCnt) << endl;
  os << "#Persoonsvorm (TW) " << presentCnt
     << " gemiddeld: " << presentCnt/double(wordCnt) << endl;
  os << "#Persoonsvorm (VERL) " << pastCnt
     << " gemiddeld: " << pastCnt/double(wordCnt) << endl;
  os << "#Clauses " << pastCnt+presentCnt
     << " gemiddeld: " << (pastCnt+presentCnt)/double(wordCnt) << endl;  
  os << "#passief constructies " << passiveCnt 
     << " gemiddeld: " << passiveCnt/double(wordCnt) << endl;
  os << "#PersoonLijke Voornaamwoorden terugverwijzend " << pronRefCnt
     << " gemiddeld: " << pronRefCnt/double(wordCnt) << endl;
  os << "#PersoonLijke Voornaamwoorden 1-ste persoon " << pron1Cnt
     << " gemiddeld: " << pron1Cnt/double(wordCnt) << endl;
  os << "#PersoonLijke Voornaamwoorden 2-de persoon " << pron2Cnt
     << " gemiddeld: " << pron2Cnt/double(wordCnt) << endl;
  os << "#PersoonLijke Voornaamwoorden 3-de persoon " << pron3Cnt
     << " gemiddeld: " << pron3Cnt/double(wordCnt) << endl;
  os << "#PersoonLijke Voornaamwoorden totaal " 
     << pron1Cnt + pron2Cnt + pron3Cnt
     << " gemiddeld: " << (pron1Cnt + pron2Cnt + pron3Cnt)/double(wordCnt) << endl;
  os << category << " gemiddelde woordlengte " << wordLen/double(wordCnt) << endl;
  os << category << " gemiddelde woordlengte zonder Namen " << wordLenExNames/double(wordCnt - nameCnt) << endl;
  os << category << " gemiddelde aantal morphemen " << morphLen/double(wordCnt) << endl;
  os << category << " gemiddelde aantal morphemen zonder Namen " << morphLenExNames/double(wordCnt-nameCnt) << endl;
  os << category << " aantal ondergeschikte bijzinnen " << onderCnt
     << " dichtheid " << onderCnt/double(wordCnt) << endl;
  os << category << " aantal betrekkelijke bijzinnen " << betrCnt
     << " dichtheid " << betrCnt/double(wordCnt) << endl;
  os << category << " aantal Namen " << nameCnt << endl;
  os << category << " Named Entities ";
  for ( map<NerProp,int>::const_iterator it = ners.begin();
	it != ners.end(); ++it ){
    os << it->first << "[" << it->second << "] ";
  }
  os << endl;
  os << category << " aantal woorden " << wordCnt << endl;
  if ( sv.size() > 0 ){
    os << category << " bevat " << sv.size() << " " 
       << sv[0]->category << " onderdelen." << endl << endl;
    for ( size_t i=0;  i < sv.size(); ++ i ){
      os << *sv[i] << endl;
    }
  }
}

void structStats::addMetrics( FoliaElement *el ) const {
  Document *doc = el->doc();
  addOneMetric( doc, el, "word_count", toString(wordCnt) );
  addOneMetric( doc, el, "name_count", toString(nameCnt) );
  addOneMetric( doc, el, "vd_count", toString(vdCnt) );
  addOneMetric( doc, el, "od_count", toString(odCnt) );
  addOneMetric( doc, el, "inf_count", toString(infCnt) );
  addOneMetric( doc, el, "archaic_count", toString(archaicsCnt) );
  addOneMetric( doc, el, "content_count", toString(contentCnt) );
  addOneMetric( doc, el, "nominal_count", toString(nominalCnt) );
  addOneMetric( doc, el, "subordinate_cnt", toString(onderCnt) );
  addOneMetric( doc, el, "relative_cnt", toString(betrCnt) );
  if ( polarity != NA )
    addOneMetric( doc, el, "polarity", toString(polarity) );
  addOneMetric( doc, el, "compound_count", toString(compCnt) );
  addOneMetric( doc, el, "compound_len", toString(compLen) );
  addOneMetric( doc, el, "word_freq", toString(lwfreq) );
  addOneMetric( doc, el, "word_freq_nonames", toString(lwfreq_n) );
  addOneMetric( doc, el, "freq50", toString(f50Cnt) );
  addOneMetric( doc, el, "freq65", toString(f65Cnt) );
  addOneMetric( doc, el, "freq77", toString(f77Cnt) );
  addOneMetric( doc, el, "freq80", toString(f80Cnt) );
  addOneMetric( doc, el, "pronoun_tw_count", toString(presentCnt) );
  addOneMetric( doc, el, "pronoun_verl_count", toString(pastCnt) );
  addOneMetric( doc, el, "pronoun_ref_count", toString(pronRefCnt) );
  addOneMetric( doc, el, "pronoun_1_count", toString(pron1Cnt) );
  addOneMetric( doc, el, "pronoun_2_count", toString(pron2Cnt) );
  addOneMetric( doc, el, "pronoun_3_count", toString(pron3Cnt) );
  addOneMetric( doc, el, "character_sum", toString(wordLen) );
  addOneMetric( doc, el, "character_sum_no_names", toString(wordLenExNames) );
  addOneMetric( doc, el, "morph_count", toString(morphLen) );
  addOneMetric( doc, el, "morph_count_no_names", toString(morphLenExNames) );
  addOneMetric( doc, el, "concrete_strict", toString(strictConcreteCnt) );
  addOneMetric( doc, el, "concrete_broad", toString(broadConcreteCnt) );
  addOneMetric( doc, el, "abstract_strict", toString(strictAbstractCnt) );
  addOneMetric( doc, el, "abstract_broad", toString(broadAbstractCnt) );
  addOneMetric( doc, el, "state_count", toString(stateCnt) );
  addOneMetric( doc, el, "action_count", toString(actionCnt) );
  addOneMetric( doc, el, "process_count", toString(processCnt) );
  addOneMetric( doc, el, "weird_count", toString(weirdCnt) );
  addOneMetric( doc, el, "human_count", toString(humanCnt) );
  addOneMetric( doc, el, "np_count", toString(npCnt) );
  addOneMetric( doc, el, "np_size", toString(npSize) );
  if ( dLevel >= 0 )
    addOneMetric( doc, el, "d_level", toString(dLevel) );

  /*
  os << category << " Named Entities ";
  for ( map<NerProp,int>::const_iterator it = ners.begin();
	it != ners.end(); ++it ){
    os << it->first << "[" << it->second << "] ";
  }
  os << endl;
  */

}

struct sentStats : public structStats {
  sentStats( Sentence *, xmlDoc * );
  virtual void print( ostream& ) const;
  void addMetrics( FoliaElement *el ) const;
};

NerProp lookupNer( const Word *w, const Sentence * s ){
  NerProp result = NONER;
  vector<Entity*> v = s->select<Entity>("http://ilk.uvt.nl/folia/sets/frog-ner-nl");
  for ( size_t i=0; i < v.size(); ++i ){
    FoliaElement *e = v[i];
    for ( size_t j=0; j < e->size(); ++j ){
      if ( e->index(j) == w ){
	//	cerr << "hit " << e->index(j) << " in " << v[i] << endl;
	string cls = v[i]->cls();
	if ( cls == "org" ){
	  if ( j == 0 )
	    result = ORG_B;
	  else
	    result = ORG_I;
	}
	else if ( cls == "eve" ){
	  if ( j == 0 )	  
	    result = EVE_B;
	  else
	    result = EVE_I;
	}
	else if ( cls == "loc" ){
	  if ( j == 0 )	  
	    result = LOC_B;
	  else
	    result = LOC_I;
	}
	else if ( cls == "misc" ){
	  if ( j == 0 )
	    result = MISC_B;
	  else
	    result = MISC_I;
	}
	else if ( cls == "per" ){
	  if ( j == 0 )	  
	    result = PER_B;
	  else
	    result = PER_I;
	}
	else if ( cls == "pro" ){
	  if ( j == 0 )	  
	    result = PRO_B;
	  else
	    result = PRO_I;
	}
	else {
	  throw ValueError( "unknown NER class: " + cls );
	}
      }
    }
  }
  return result;
}

void np_length( Sentence *s, int& count, int& size ) {
  vector<Chunk *> cv = s->select<Chunk>();
  size = 0 ;
  for( size_t i=0; i < cv.size(); ++i ){
    if ( cv[i]->cls() == "NP" ){
      ++count;
      size += cv[i]->size();
    }
  }
}

const string neg_longA[] = { "afgezien van", 
			     "zomin als",
			     "met uitzondering van"};
set<string> negatives_long = set<string>( neg_longA, neg_longA + 3 );

sentStats::sentStats( Sentence *s, xmlDoc *alpDoc ): structStats("ZIN" ){
  id = s->id();
  text = UnicodeToUTF8( s->toktext() );
  vector<Word*> w = s->words();
  for ( size_t i=0; i < w.size(); ++i ){
    wordStats *ws = new wordStats( w[i], alpDoc );
    if ( ws->prop == ISPUNCT ){
      delete ws;
      continue;
    }
    else {
      NerProp ner = lookupNer( w[i], s );
      switch( ner ){
      case LOC_B:
      case EVE_B:
      case MISC_B:
      case ORG_B:
      case PER_B:
      case PRO_B:
	ners[ner]++;
	break;
      default:
	;
      }
      wordCnt++;
      heads[ws->posHead]++;
      wordLen += ws->wordLen;
      wordLenExNames += ws->wordLenExNames;
      morphLen += ws->morphLen;
      morphLenExNames += ws->morphLenExNames;
      unique_words[lowercase(ws->word)] += 1;
      unique_lemmas[ws->lemma] += 1;
      if ( ws->compLen > 0 )
	++compCnt;
      compLen += ws->compLen;
      wfreq += ws->wfreq;
      if ( ws->prop != ISNAME )
	wfreq_n += ws->wfreq;
      switch ( ws->prop ){
      case ISNAME:
	nameCnt++;
	break;
      case ISVD:
	vdCnt++;
	break;
      case ISINF:
	infCnt++;
	break;
      case ISOD:
	odCnt++;
	break;
      case ISPVVERL:
	pastCnt++;
	break;
      case ISPVTGW:
	presentCnt++;
	break;
      case ISPPRON1:
	pron1Cnt++;
      case ISPPRON2:
	pron2Cnt++;
      case ISPPRON3:
	pron3Cnt++;
      default:
	;// ignore
      }
      if ( ws->isPassive )
	passiveCnt++;
      if ( ws->isPronRef )
	pronRefCnt++;
      if ( ws->archaic )
	archaicsCnt++;
      if ( ws->isContent )
	contentCnt++;
      if ( ws->isNominal )
	nominalCnt++;
      if ( ws->isOnder )
	onderCnt++;
      if ( ws->isBetr )
	betrCnt++;
      if ( ws->isPropNeg )
	propNegCnt++;
      if ( ws->isMorphNeg )
	morphNegCnt++;
      if ( ws->f50 )
	f50Cnt++;
      if ( ws->f65 )
	f65Cnt++;
      if ( ws->f77 )
	f77Cnt++;
      if ( ws->f80 )
	f80Cnt++;
      if ( ws->polarity != NA ){
	if ( polarity == NA )
	  polarity = ws->polarity;
	else
	  polarity += ws->polarity;
      }
      switch ( ws->sem_type ){
      case CONCRETE_HUMAN:
	humanCnt++;
	// fall throug
      case CONCRETE:
	strictConcreteCnt++;
	broadConcreteCnt++;
	break;
      case ABSTRACT:
	strictAbstractCnt++;
	broadAbstractCnt++;
	break;
      case BROAD:
	broadConcreteCnt++;
	broadAbstractCnt++;
	break;
      case STATE:
	stateCnt++;
	break;
      case ACTION:
	actionCnt++;
	break;
      case PROCESS:
	processCnt++;
	break;
      case WEIRD:
	weirdCnt++;
	break;
      default:
	;
      }
      sv.push_back( ws );
    }
  }
  if ( w.size() > 1 ){
    for ( size_t i=0; i < sv.size()-2; ++i ){
      string multiword2 = lowercase( UnicodeToUTF8( w[i]->text() ) )
	+ lowercase( UnicodeToUTF8( w[i+1]->text() ) );
      if ( negatives_long.find( multiword2 ) != negatives_long.end() ){
	propNegCnt++;
      }
      else {
	string multiword3 = multiword2 
	  + lowercase( UnicodeToUTF8( w[i+2]->text() ) );
	if ( negatives_long.find( multiword3 ) != negatives_long.end() )
	  propNegCnt++;
      }
    }
    string multiword2 = lowercase( UnicodeToUTF8( w[w.size()-2]->text() ) )
      + lowercase( UnicodeToUTF8( w[w.size()-1]->text() ) );
    if ( negatives_long.find( multiword2 ) != negatives_long.end() ){
      propNegCnt++;
    }
  }
  lwfreq = log( wfreq / w.size() );
  lwfreq_n = log( wfreq_n / (wordCnt-nameCnt) );
  np_length( s, npCnt, npSize );
  if ( alpDoc )
    dLevel = get_d_level( s, alpDoc );
  addMetrics( s );
}

void sentStats::print( ostream& os ) const {
  os << category << "[" << text << "]" << endl;
  os << category << " er zijn " << propNegCnt << " negatieve woorden en " 
     << morphNegCnt << " negatieve morphemen gevonden";
  if ( propNegCnt + morphNegCnt > 1 )
    os << " er is dus een dubbele ontkenning! ";
  os << endl;
  structStats::print( os );
}

void sentStats::addMetrics( FoliaElement *el ) const {
  structStats::addMetrics( el );
}

struct parStats: public structStats {
  parStats( Paragraph * );
  void print( ostream& ) const;
  void addMetrics( FoliaElement *el ) const;
  int sentCnt;
};

parStats::parStats( Paragraph *p ): 
  structStats( "PARAGRAAF" ), 
  sentCnt(0) 
{
  id = p->id();
  vector<Sentence*> sents = p->sentences();
  for ( size_t i=0; i < sents.size(); ++i ){
    xmlDoc *alpDoc = 0;
    if ( settings.doAlpino ){
      cerr << "calling Alpino parser" << endl;
      alpDoc = AlpinoParse( sents[i] );
      if ( !alpDoc ){
	cerr << "alpino parser failed!" << endl;
      }
    }
    sentStats *ss = new sentStats( sents[i], alpDoc );
    xmlFreeDoc( alpDoc );
    merge( ss );
    sentCnt++;
  }
  lwfreq = log( wfreq / sents.size() );
  lwfreq_n = log( wfreq_n / (wordCnt-nameCnt) );
  addMetrics( p );
}

void parStats::print( ostream& os ) const {
  os << category << " with " << sentCnt << " Sentences" << endl;
  structStats::print( os );
}

void parStats::addMetrics( FoliaElement *el ) const {
  structStats::addMetrics( el );
  addOneMetric( el->doc(), el, 
		"sentence_count", toString(sentCnt) );
}

struct docStats : public structStats {
  docStats( Document * );
  void print( ostream& ) const;
  void addMetrics( FoliaElement *el ) const;
  int sentCnt;
};

docStats::docStats( Document *doc ):
  structStats( "DOCUMENT" ), sentCnt(0) 
{
  doc->declare( AnnotationType::METRIC, 
		"metricset", 
		"annotator='tscan'" );
  doc->declare( AnnotationType::POS, 
		"tscan-set", 
		"annotator='tscan'" );
  if ( !settings.style.empty() ){
    doc->replaceStyle( "text/xsl", settings.style );
  }
  vector<Paragraph*> pars = doc->paragraphs();
  for ( size_t i=0; i != pars.size(); ++i ){
    parStats *ps = new parStats( pars[i] );
    merge( ps );
    sentCnt += ps->sentCnt;
  }
  lwfreq = log( wfreq / pars.size() );
  lwfreq_n = log( wfreq_n / (wordCnt-nameCnt) );
  addMetrics( pars[0]->parent() );
}

double rarity( const docStats *d, double level ){
  map<string,int>::const_iterator it = d->unique_lemmas.begin();
  int rare = 0;
  while ( it != d->unique_lemmas.end() ){
    if ( it->second <= level )
      ++rare;
    ++it;
  }
  return rare/double( d->unique_lemmas.size() );
}

void docStats::addMetrics( FoliaElement *el ) const {
  structStats::addMetrics( el );
  addOneMetric( el->doc(), el, 
		"sentence_count", toString( sentCnt ) );
  addOneMetric( el->doc(), el, 
		"paragraph_count", toString( sv.size() ) );
  addOneMetric( el->doc(), el, 
		"TTW", toString( unique_words.size()/double(wordCnt) ) );
  addOneMetric( el->doc(), el, 
		"TTL", toString( unique_lemmas.size()/double(wordCnt) ) );
  addOneMetric( el->doc(), el, 
		"rarity", toString( rarity( this, settings.rarityLevel ) ) );
}

void docStats::print( ostream& os ) const {
  os << category << " with "  << sv.size() << " paragraphs and "
     << sentCnt << " Sentences" << endl;
  os << "TTW = " << unique_words.size()/double(wordCnt) << endl;
  os << "TTL = " << unique_lemmas.size()/double(wordCnt) << endl;
  os << "rarity(" << settings.rarityLevel << ")=" 
     << rarity( this, settings.rarityLevel ) << endl;
  structStats::print( os );
}

Document *getFrogResult( istream& is ){
  string host = config.lookUp( "host", "frog" );
  string port = config.lookUp( "port", "frog" );
  Sockets::ClientSocket client;
  if ( !client.connect( host, port ) ){
    LOG << "failed to open Frog connection: "<< host << ":" << port << endl;
    LOG << "Reason: " << client.getMessage() << endl;
    return 0;
  }
  DBG << "start input loop" << endl;
  string line;
  while ( getline( is, line ) ){
    DBG << "read: " << line << endl;
    client.write( line + "\n" );
  }
  client.write( "\nEOT\n" );
  string result;
  string s;
  while ( client.read(s) ){
    if ( s == "READY" )
      break;
    result += s + "\n";
  }
  DBG << "received data [" << result << "]" << endl;
  Document *doc = 0;
  if ( !result.empty() && result.size() > 10 ){
    DBG << "start FoLiA parsing" << endl;
    doc = new Document();
    try {
      doc->readFromString( result );
      DBG << "finished" << endl;
    }
    catch ( std::exception& e ){
      LOG << "FoLiaParsing failed:" << endl
	   << e.what() << endl;	  
    }
  }
  return doc;
}

void exec( const string& file, ostream& os ){
  cerr << "TScan " << VERSION << endl;
  LOG << "try file ='" << file << "'" << endl;
  ifstream is( file.c_str() );
  if ( !is ){
    os << "failed to open file '" << file << "'" << endl;
  }
  else {
    os << "opened file " << file << endl;
    Document *doc = getFrogResult( is );
    if ( !doc ){
      os << "big trouble: no FoLiA document created " << endl;
    }
    else {
      doc->save( "/tmp/folia.1.xml" );
      docStats result( doc );
      doc->save( "/tmp/folia.2.xml" );
      delete doc;
      os << result << endl;
    }
  }
}
  
int main(int argc, char *argv[]) {
  Timbl::TimblOpts opts( argc, argv );
  string val;
  bool mood;
  if ( opts.Find( "h", val, mood ) ||
       opts.Find( "help", val, mood ) ){
    usage();
    exit( EXIT_SUCCESS );
  }
  if ( opts.Find( "V", val, mood ) ||
       opts.Find( "version", val, mood ) ){
    exit( EXIT_SUCCESS );
  }

  if ( opts.Find( "config", val, mood ) ){
    configFile = val;
    opts.Delete( "config" );
  }
  if ( !configFile.empty() &&
       config.fill( configFile ) ){
    settings.init( config );
  }
  else {
    cerr << "invalid configuration" << endl;
    exit( EXIT_FAILURE );
  }
  if ( opts.Find( 'D', val, mood ) ){
    if ( val == "Normal" )
      cur_log.setlevel( LogNormal );
    else if ( val == "Debug" )
      cur_log.setlevel( LogDebug );
    else if ( val == "Heavy" )
      cur_log.setlevel( LogHeavy );
    else if ( val == "Extreme" )
      cur_log.setlevel( LogExtreme );
    else {
      cerr << "Unknown Debug mode! (-D " << val << ")" << endl;
    }
    opts.Delete( 'D' );
  }
  if ( opts.Find( 't', val, mood ) ){
    exec( val, cout );
  }
  else {
    cerr << "missing input file (-t option) " << endl;
  }

  exit(EXIT_SUCCESS);
}
