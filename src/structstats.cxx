#include "tscan/stats.h"

using namespace std;

structStats::~structStats(){
  vector<basicStats *>::iterator it = sv.begin();
  while ( it != sv.end() ){
    delete( *it );
    ++it;
  }
}

vector<const wordStats*> structStats::collectWords() const {
  vector<const wordStats*> result;
  vector<basicStats *>::const_iterator it = sv.begin();
  while ( it != sv.end() ){
    vector<const wordStats*> tmp = (*it)->collectWords();
    result.insert( result.end(), tmp.begin(), tmp.end() );
    ++it;
  }
  return result;
}

/****
 * AL
 ****/

double structStats::getMeanAL() const {
  double sum = 0;
  for ( size_t i=0; i < sv.size(); ++i ){
    double val = sv[i]->get_al_gem();
    if ( !std::isnan(val) ){
      sum += val;
    }
  }
  if ( sum == 0 )
    return NAN;
  else
    return sum/sv.size();
}

double structStats::getHighestAL() const {
  double sum = 0;
  for ( size_t i=0; i < sv.size(); ++i ){
    double val = sv[i]->get_al_max();
    if ( !std::isnan(val) ){
      sum += val;
    }
  }
  if ( sum == 0 )
    return NAN;
  else
    return sum/sv.size();
}

/************
 * CSV OUTPUT
 ************/

/**
 * Sets all headers of the structStats .csv-output.
 * @param os    the current outputstream
 * @param intro specific columns per struct (document, paragraph, sentence)
 */
void structStats::CSVheader( ostream& os, const string& intro ) const {
  os << intro << ",Alpino_status,";
  wordDifficultiesHeader( os );
  compoundHeader( os );
  sentDifficultiesHeader( os );
  infoHeader( os );
  coherenceHeader( os );
  concreetHeader( os );
  persoonlijkheidHeader( os );
  verbHeader( os );
  imperativeHeader( os );
  wordSortHeader( os );
  prepPhraseHeader( os );
  intensHeader( os );
  miscHeader( os );
  os << endl;
}

/**
 * Sets all .csv-output for structStats.
 * @param os the current outputstream
 */
void structStats::toCSV( ostream& os ) const {
  if (!isSentence())
  {
    // For paragraphs and documents, add a sentence and word count.
    os << sentCnt << ",";
    os << wordCnt << ",";
  }
  else
  {
    // For sentences, add the original sentence (quoted)
    os << "\"" << escape_quotes(text) << "\",";
  }

  os << parseFailCnt << ",";

  wordDifficultiesToCSV( os );
  compoundToCSV( os );
  sentDifficultiesToCSV( os );
  informationDensityToCSV( os );
  coherenceToCSV( os );
  concreetToCSV( os );
  persoonlijkheidToCSV( os );
  verbToCSV( os );
  imperativeToCSV( os );
  wordSortToCSV( os );
  prepPhraseToCSV( os );
  intensToCSV( os );
  miscToCSV( os );

  os << endl;
}

void structStats::wordDifficultiesHeader( ostream& os ) const {
  os << "Let_per_wrd,Wrd_per_let,Let_per_wrd_zn,Wrd_per_let_zn,"
     << "Morf_per_wrd,Wrd_per_morf,Morf_per_wrd_zn,Wrd_per_morf_zn,"
     << "Namen_p,Namen_d,"
     << "Wrd_prev,Wrd_prev_z,Inhwrd_prev,Inhwrd_prev_z,Dekking_inhwrd_prev,"
     << "Freq50_staph,Freq65_Staph,Freq77_Staph,Freq80_Staph,"
     << "Wrd_freq_log,Wrd_freq_zn_log,Lem_freq_log,Lem_freq_zn_log,"
     << "Wrd_freq_log_zonder_abw,Wrd_freq_zn_log_zonder_abw,Lem_freq_log_zonder_abw,Lem_freq_zn_log_zonder_abw,"
     << "Freq1000,Freq2000,Freq3000,"
     << "Freq5000,Freq10000,Freq20000,"
     << "Freq1000_inhwrd,Freq2000_inhwrd,Freq3000_inhwrd,"
     << "Freq5000_inhwrd,Freq10000_inhwrd,Freq20000_inhwrd,"
     << "Freq1000_inhwrd_zonder_abw,Freq2000_inhwrd_zonder_abw,Freq3000_inhwrd_zonder_abw,"
     << "Freq5000_inhwrd_zonder_abw,Freq10000_inhwrd_zonder_abw,Freq20000_inhwrd_zonder_abw,";
}

void structStats::wordDifficultiesToCSV( ostream& os ) const {
  os << std::showpoint
     << proportion( charCnt, wordCnt ) << ","
     << proportion( wordCnt, charCnt ) <<  ","
     << proportion( charCntExNames, (wordCnt-nameCnt) ) << ","
     << proportion( (wordCnt - nameCnt), charCntExNames ) <<  ","
     << proportion( morphCnt, wordCnt ) << ","
     << proportion( wordCnt, morphCnt ) << ","
     << proportion( morphCntExNames, (wordCnt-nameCnt) ) << ","
     << proportion( (wordCnt-nameCnt), morphCntExNames ) << ","

     << proportion( nameCnt, (nameCnt+nounCnt) ) << ","
     << density( nameCnt, wordCnt ) << ",";

  os << proportion( prevalenceP, prevalenceCovered ) << ",";
  os << proportion( prevalenceZ, prevalenceCovered ) << ",";
  os << proportion( prevalenceContentP, prevalenceContentCovered ) << ",";
  os << proportion( prevalenceContentZ, prevalenceContentCovered ) << ",";
  os << proportion( prevalenceContentCovered, contentCnt ) << ",";

  os << proportion( f50Cnt, wordCnt ) << ",";
  os << proportion( f65Cnt, wordCnt ) << ",";
  os << proportion( f77Cnt, wordCnt ) << ",";
  os << proportion( f80Cnt, wordCnt ) << ",";

  os << word_freq_log << ",";
  os << word_freq_log_n << ",";
  os << lemma_freq_log << ",";
  os << lemma_freq_log_n << ",";

  os << word_freq_log_strict << ",";
  os << word_freq_log_n_strict << ",";
  os << lemma_freq_log_strict << ",";
  os << lemma_freq_log_n_strict << ",";

  os << proportion( top1000Cnt, wordCnt ) << ",";
  os << proportion( top2000Cnt, wordCnt ) << ",";
  os << proportion( top3000Cnt, wordCnt ) << ",";
  os << proportion( top5000Cnt, wordCnt ) << ",";
  os << proportion( top10000Cnt, wordCnt ) << ",";
  os << proportion( top20000Cnt, wordCnt ) << ",";

  os << proportion( top1000ContentCnt, contentCnt ) << ",";
  os << proportion( top2000ContentCnt, contentCnt ) << ",";
  os << proportion( top3000ContentCnt, contentCnt ) << ",";
  os << proportion( top5000ContentCnt, contentCnt ) << ",";
  os << proportion( top10000ContentCnt, contentCnt ) << ",";
  os << proportion( top20000ContentCnt, contentCnt ) << ",";

  os << proportion( top1000ContentStrictCnt, contentStrictCnt ) << ",";
  os << proportion( top2000ContentStrictCnt, contentStrictCnt ) << ",";
  os << proportion( top3000ContentStrictCnt, contentStrictCnt ) << ",";
  os << proportion( top5000ContentStrictCnt, contentStrictCnt ) << ",";
  os << proportion( top10000ContentStrictCnt, contentStrictCnt ) << ",";
  os << proportion( top20000ContentStrictCnt, contentStrictCnt ) << ",";
}

void structStats::compoundHeader( ostream& os ) const {
  os << "Samenst_d,Samenst_p,Samenst3_d,Samenst3_p,"; 
  os << "Let_per_wrd_nw,Let_per_wrd_nsam,Let_per_wrd_sam,"; 
  os << "Let_per_wrd_hfdwrd,Let_per_wrd_satwrd,"; 
  os << "Let_per_wrd_nw_corr,Let_per_wrd_corr,"; 
  os << "Wrd_freq_log_nw,Wrd_freq_log_ong_nw,Wrd_freq_log_sam_nw,"; 
  os << "Wrd_freq_log_hfdwrd,Wrd_freq_log_satwrd,Wrd_freq_log_(hfd_sat),"; 
  os << "Wrd_freq_log_nw_corr,Wrd_freq_log_corr,"; 
  os << "Wrd_freq_log_zn_corr,";
  os << "Wrd_freq_log_corr_zonder_abw,";
  os << "Wrd_freq_log_zn_corr_zonder_abw,";
  os << "Freq1000_nw,Freq5000_nw,Freq20000_nw,";
  os << "Freq1000_nsam_nw,Freq5000_nsam_nw,Freq20000_nsam_nw,";
  os << "Freq1000_sam_nw,Freq5000_sam_nw,Freq20000_sam_nw,";
  os << "Freq1000_hfdwrd_nw,Freq5000_hfdwrd_nw,Freq20000_hfdwrd_nw,";
  os << "Freq1000_satwrd_nw,Freq5000_satwrd_nw,Freq20000_satwrd_nw,";
  os << "Freq1000_nw_corr,Freq5000_nw_corr,Freq20000_nw_corr,";
  os << "Freq1000_corr,Freq5000_corr,Freq20000_corr,";
}

void structStats::compoundToCSV( ostream& os ) const {
  int nonCompoundCnt = nounCnt - compoundCnt;
  os << density(compoundCnt, wordCnt) << ",";
  os << proportion(compoundCnt, nounCnt) << ",";
  os << density(compound3Cnt, wordCnt) << ",";
  os << proportion(compound3Cnt, nounCnt) << ",";
  os << proportion(charCntNoun, nounCnt) << ",";
  os << proportion(charCntNonComp, nonCompoundCnt) << ",";
  os << proportion(charCntComp, compoundCnt) << ",";
  os << proportion(charCntHead, compoundCnt) << ",";
  os << proportion(charCntSat, compoundCnt) << ",";
  os << proportion(charCntNounCorr, nounCnt) << ",";
  os << proportion(charCntCorr, wordCnt) << ",";
  os << proportion(word_freq_log_noun, nounCnt) << ",";
  os << proportion(word_freq_log_non_comp, nonCompoundCnt) << ",";
  os << proportion(word_freq_log_comp, compoundCnt) << ",";
  os << proportion(word_freq_log_head, compoundCnt) << ",";
  os << proportion(word_freq_log_sat, compoundCnt) << ",";
  os << proportion(word_freq_log_head_sat, compoundCnt) << ",";
  os << proportion(word_freq_log_noun_corr, nounCnt) << ",";
  os << proportion(word_freq_log_corr, contentCnt) << ",";

  os << proportion(word_freq_log_n_corr, contentCnt-nameCnt) << ",";
  os << proportion(word_freq_log_corr_strict, contentStrictCnt) << ",";
  os << proportion(word_freq_log_n_corr_strict, contentStrictCnt-nameCnt) << ",";

  os << proportion(top1000CntNoun, nounCnt) << ",";
  os << proportion(top5000CntNoun, nounCnt) << ",";
  os << proportion(top20000CntNoun, nounCnt) << ",";
  os << proportion(top1000CntNonComp, nonCompoundCnt) << ",";
  os << proportion(top5000CntNonComp, nonCompoundCnt) << ",";
  os << proportion(top20000CntNonComp, nonCompoundCnt) << ",";
  os << proportion(top1000CntComp, compoundCnt) << ",";
  os << proportion(top5000CntComp, compoundCnt) << ",";
  os << proportion(top20000CntComp, compoundCnt) << ",";
  os << proportion(top1000CntHead, compoundCnt) << ",";
  os << proportion(top5000CntHead, compoundCnt) << ",";
  os << proportion(top20000CntHead, compoundCnt) << ",";
  os << proportion(top1000CntSat, compoundCnt) << ",";
  os << proportion(top5000CntSat, compoundCnt) << ",";
  os << proportion(top20000CntSat, compoundCnt) << ",";
  os << proportion(top1000CntNounCorr, nounCnt) << ",";
  os << proportion(top5000CntNounCorr, nounCnt) << ",";
  os << proportion(top20000CntNounCorr, nounCnt) << ",";
  os << proportion(top1000CntCorr, wordCnt) << ",";
  os << proportion(top5000CntCorr, wordCnt) << ",";
  os << proportion(top20000CntCorr, wordCnt) << ",";
}

void structStats::sentDifficultiesHeader( ostream& os ) const {
  os << "Wrd_per_zin,Wrd_per_dz,Zin_per_wrd,Dzin_per_wrd,"
     << "Wrd_per_nwg,"
     << "Betr_bijzin_per_zin,Bijw_bijzin_per_zin,"
     << "Compl_bijzin_per_zin,Fin_bijzin_per_zin,"
     << "Mv_fin_inbed_per_zin,Infin_compl_per_zin,"
     << "Bijzin_per_zin,Mv_inbed_per_zin,"
     << "Betr_bijzin_los,Bijw_compl_bijzin_los,"
     << "Pv_hzin_per_zin,Pv_bijzin_per_zin,Pv_ww1_per_zin,"
     << "Hzin_conj,Bijzin_conj,Ww1_conj,"
     << "Pv_Alpino_per_zin,"
     << "Pv_Frog_d,Pv_Frog_per_zin,";
  if ( isSentence() ){
    os << "D_level,";
  }
  else {
    os  << "D_level,D_level_gt4_p,";
  }
  os << "Nom_d,Lijdv_d,Lijdv_dz,Ontk_zin_d,Ontk_zin_dz,"
     << "Ontk_morf_d,Ontk_morf_dz,Ontk_tot_d,Ontk_tot_dz,"
     << "Meerv_ontk_d,Meerv_ontk_dz,"
     << "AL_sub_ww,AL_ob_ww,AL_indirob_ww,AL_ww_vzg,"
     << "AL_lidw_znw,AL_vz_znw,AL_ww_wwvc,"
     << "AL_vg_wwbijzin,AL_vg_conj,AL_vg_wwhoofdzin,AL_znw_bijzin,AL_ww_schdw,"
     << "AL_ww_znwpred,AL_ww_bnwpred,AL_ww_bnwbwp,AL_ww_bwbwp,AL_ww_znwbwp,"
     << "AL_gem,AL_max,";
}

void structStats::sentDifficultiesToCSV( ostream& os ) const {
  if ( parseFailCnt > 0 ) {
    os << "NA,";
  }
  else {
    os << proportion( wordInclCnt, sentCnt ) << ",";
  }

  os << proportion( wordInclCnt, correctedClauseCnt ) << ",";
  os << proportion( sentCnt, wordInclCnt )  << ",";
  os << proportion( correctedClauseCnt, wordInclCnt )  << ",";
  os << proportion( wordInclCnt, npCnt ) << ",";

  double bijzinCnt = betrCnt + bijwCnt + complCnt;
  if ( parseFailCnt > 0 ) {
    os << "NA,NA,NA,NA,NA,NA,NA,NA,NA,NA,";
  }
  else {
    os << proportion( betrCnt, sentCnt ) << ",";
    os << proportion( bijwCnt, sentCnt ) << ",";
    os << proportion( complCnt, sentCnt ) << ",";
    os << proportion( bijzinCnt, sentCnt ) << ",";
    os << proportion( mvFinInbedCnt, sentCnt ) << ",";
    os << proportion( infinComplCnt, sentCnt ) << ",";
    os << proportion( bijzinCnt + infinComplCnt, sentCnt ) << ",";
    os << proportion( mvInbedCnt, sentCnt ) << ",";
    os << proportion( losBetrCnt, sentCnt ) << ",";
    os << proportion( losBijwCnt, sentCnt ) << ",";
  }

  if ( parseFailCnt > 0 ) {
    os << "NA,NA,NA,NA,NA,NA,NA,";
  }
  else {
    os << proportion( smainCnt, sentCnt ) << ",";
    os << proportion( ssubCnt, sentCnt ) << ",";
    os << proportion( sv1Cnt, sentCnt ) << ",";
    os << proportion( smainCnjCnt, sentCnt ) << ",";
    os << proportion( ssubCnjCnt, sentCnt ) << ",";
    os << proportion( sv1CnjCnt, sentCnt ) << ",";
    os << proportion( clauseCnt, sentCnt ) << ",";
  }

  double frogClauseCnt = pastCnt + presentCnt;
  os << density( frogClauseCnt, wordInclCnt ) << ",";
  os << proportion( frogClauseCnt, sentCnt ) << ",";

  os << proportion( dLevel, sentCnt ) << ",";
  if ( !isSentence() ){
    os << proportion( dLevel_gt4, sentCnt ) << ",";
  }
  os << density( nominalCnt, wordCnt ) << ",";
  os << density( passiveCnt, wordInclCnt ) << ",";
  os << proportion( passiveCnt, correctedClauseCnt ) << ",";
  os << density( propNegCnt, wordInclCnt ) << ",";
  os << proportion( propNegCnt, correctedClauseCnt ) << ",";
  os << density( morphNegCnt, wordInclCnt ) << ",";
  os << proportion( morphNegCnt, correctedClauseCnt ) << ",";
  os << density( propNegCnt+morphNegCnt, wordInclCnt ) << ",";
  os << proportion( propNegCnt+morphNegCnt, correctedClauseCnt ) << ",";
  os << density( multiNegCnt, wordInclCnt ) << ",";
  os << proportion( multiNegCnt, correctedClauseCnt ) << ",";
  os << MMtoString( distances, SUB_VERB ) << ",";
  os << MMtoString( distances, OBJ1_VERB ) << ",";
  os << MMtoString( distances, OBJ2_VERB ) << ",";
  os << MMtoString( distances, VERB_PP ) << ",";
  os << MMtoString( distances, NOUN_DET ) << ",";
  os << MMtoString( distances, PREP_OBJ1 ) << ",";
  os << MMtoString( distances, VERB_VC ) << ",";
  os << MMtoString( distances, COMP_BODY ) << ",";
  os << MMtoString( distances, CRD_CNJ ) << ",";
  os << MMtoString( distances, VERB_COMP ) << ",";
  os << MMtoString( distances, NOUN_VC ) << ",";
  os << MMtoString( distances, VERB_SVP ) << ",";
  os << MMtoString( distances, VERB_PREDC_N ) << ",";
  os << MMtoString( distances, VERB_PREDC_A ) << ",";
  os << MMtoString( distances, VERB_MOD_A ) << ",";
  os << MMtoString( distances, VERB_MOD_BW ) << ",";
  os << MMtoString( distances, VERB_NOUN ) << ",";
  os << toMString( al_gem ) << ",";
  os << toMString( al_max ) << ",";
}

void structStats::infoHeader( ostream& os ) const {
  os << "Bijw_bep_d,Bijw_bep_dz,Bijw_bep_dz_zbijzin,"
     << "Bijw_bep_alg_d,Bijw_bep_alg_dz,"
     << "Bijv_bep_d,Bijv_bep_dz,Bijv_bep_dz_zbijzin,"
     << "Attr_bijv_nw_d,Attr_bijv_nw_dz,"
     << "Ov_bijv_bep_d,Ov_bijv_bep_dz,"
     << "KConj_per_zin,Extra_KConj_per_zin,"
     << "KConj_dz,Extra_KConj_dz,"
     << "Props_dz_tot,"
     << "TTR_wrd,MTLD_wrd,TTR_lem,MTLD_lem,"
     << "TTR_namen,MTLD_namen,"
     << "TTR_inhwrd,MTLD_inhwrd,"
     << "TTR_inhwrd_zonder_abw,MTLD_inhwrd_zonder_abw,"
     << "Inhwrd_d,Inhwrd_dz,"
     << "Inhwrd_d_zonder_abw,Inhwrd_dz_zonder_abw,"
     << "Zeldz_index,"
     << "Vnw_ref_d,Vnw_ref_dz,"
     << "Arg_over_vzin_d,Arg_over_vzin_dz,Lem_over_vzin_d,Lem_over_vzin_dz,"
     << "Arg_over_buf_d,Arg_over_buf_dz,Lem_over_buf_d,Lem_over_buf_dz,"
     << "Onbep_nwg_p,Onbep_nwg_dz,";
}

void structStats::informationDensityToCSV( ostream& os ) const {
  os << density( vcModCnt, wordInclCnt ) << ",";
  os << proportion( vcModCnt, correctedClauseCnt ) << ",";

  int vcModCorrectedCnt = max(0, vcModCnt - bijwCnt);
  os << proportion( vcModCorrectedCnt, correctedClauseCnt ) << ",";

  os << density( vcModSingleCnt, wordInclCnt ) << ",";
  os << proportion( vcModSingleCnt, correctedClauseCnt ) << ",";

  os << density( npModCnt, wordInclCnt ) << ",";
  os << proportion( npModCnt, correctedClauseCnt ) << ",";

  int npModCorrectedCnt = max(0, npModCnt - betrCnt);
  os << proportion( npModCorrectedCnt, correctedClauseCnt ) << ",";

  os << density( adjNpModCnt, wordInclCnt ) << ",";
  os << proportion( adjNpModCnt, correctedClauseCnt ) << ",";

  os << density( npModCnt-adjNpModCnt, wordInclCnt ) << ",";
  os << proportion( npModCnt-adjNpModCnt, correctedClauseCnt ) << ",";

  os << proportion( smallCnjCnt, sentCnt ) << ",";
  os << proportion( smallCnjExtraCnt, sentCnt ) << ",";
  os << proportion( smallCnjCnt, correctedClauseCnt ) << ",";
  os << proportion( smallCnjExtraCnt, correctedClauseCnt ) << ",";

  int propositionCount = vcModCorrectedCnt + npModCorrectedCnt + smallCnjExtraCnt;
  double propositionPr = proportion( propositionCount, correctedClauseCnt ).p + 1.0;
  os << propositionPr << ",";

  os << proportion( unique_words.size(), wordInclCnt ) << ",";
  os << word_mtld << ",";

  os << proportion( unique_lemmas.size(), wordInclCnt ) << ",";
  os << lemma_mtld << ",";

  os << proportion( unique_names.size(), nameInclCnt ) << ",";
  os << name_mtld << ",";

  os << proportion( unique_contents.size(), contentInclCnt ) << ",";
  os << content_mtld << ",";

  os << proportion( unique_contents_strict.size(), contentStrictInclCnt ) << ",";
  os << content_mtld_strict << ",";

  os << density( contentInclCnt, wordInclCnt ) << ",";
  os << proportion( contentInclCnt, correctedClauseCnt ) << ",";

  os << density( contentStrictInclCnt, wordInclCnt ) << ",";
  os << proportion( contentStrictInclCnt, correctedClauseCnt ) << ",";

  double rare = rarity( rarityLevel );
  if (std::isnan(rare)) {
    os << "NA" << ",";
  }
  else {
    os << rare << ",";
  }
  
  os << density( pronRefCnt, wordInclCnt ) << ",";
  os << proportion( pronRefCnt, correctedClauseCnt ) << ",";
  if ( isSentence() ){
    if ( index == 0 ){
      os << "NA,NA,"
   << "NA,NA,";
    }
    else {
      os << density( wordOverlapCnt, wordInclCnt ) << ",NA,"
   << density( lemmaOverlapCnt, wordInclCnt ) << ",NA,";
    }
  }
  else {
    os << density( wordOverlapCnt, wordInclCnt ) << ",";
    os << proportion( wordOverlapCnt, correctedClauseCnt ) << ",";
    os << density( lemmaOverlapCnt, wordInclCnt ) << ",";
    os << proportion( lemmaOverlapCnt, correctedClauseCnt ) << ",";
  }
  if ( !isDocument() ){
    os << "NA,NA,NA,NA,";
  }
  else {
    os << density( word_overlapCnt(), wordInclCnt - overlapSize ) << ",";
    os << proportion( word_overlapCnt(), correctedClauseCnt ) << ",";
    os << density( lemma_overlapCnt(), wordInclCnt - overlapSize ) << ",";
    os << proportion( lemma_overlapCnt(), correctedClauseCnt ) << ",";
  }
  os << proportion( indefNpCnt, npCnt ) << ",";
  os << proportion( indefNpCnt, correctedClauseCnt ) << ",";
}

void structStats::coherenceHeader( ostream& os ) const {
  os << "Conn_d,Conn_dz,Conn_TTR,Conn_MTLD,"
     << "Conn_temp_d,Conn_temp_dz,Conn_temp_TTR,Conn_temp_MTLD,"
     << "Conn_reeks_wg_d,Conn_reeks_wg_dz,Conn_reeks_wg_TTR,Conn_reeks_wg_MTLD,"
     << "Conn_reeks_zin_d,Conn_reeks_zin_dz,Conn_reeks_zin_TTR,Conn_reeks_zin_MTLD,"
     << "Conn_contr_d,Conn_contr_dz,Conn_contr_TTR,Conn_contr_MTLD,"
     << "Conn_comp_d,Conn_comp_dz,Conn_comp_TTR,Conn_comp_MTLD,"
     << "Conn_caus_d,Conn_caus_dz,Conn_caus_TTR,Conn_caus_MTLD,"
     << "Causaal_d,Ruimte_d,Tijd_d,Emotie_d,"
     << "Causaal_TTR,Causaal_MTLD,"
     << "Ruimte_TTR,Ruimte_MTLD,"
     << "Tijd_TTR,Tijd_MTLD,"
     << "Emotie_TTR,Emotie_MTLD,";
}

void structStats::coherenceToCSV( ostream& os ) const {
  os << density( allConnCnt, wordInclCnt ) << ",";
  os << proportion( allConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_all_conn.size(), allConnCnt ) << ",";
  os << all_conn_mtld << ",";
  os << density( tempConnCnt, wordInclCnt ) << ",";
  os << proportion( tempConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_temp_conn.size(), tempConnCnt ) << ",";
  os << temp_conn_mtld << ",";
  os << density( opsomWgConnCnt, wordInclCnt ) << ",";
  os << proportion( opsomWgConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_reeks_wg_conn.size(), opsomWgConnCnt ) << ",";
  os << reeks_zin_conn_mtld << ",";
  os << density( opsomZinConnCnt, wordInclCnt ) << ",";
  os << proportion( opsomZinConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_reeks_zin_conn.size(), opsomZinConnCnt ) << ",";
  os << reeks_zin_conn_mtld << ",";
  os << density( contrastConnCnt, wordInclCnt ) << ",";
  os << proportion( contrastConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_contr_conn.size(), contrastConnCnt ) << ",";
  os << contr_conn_mtld << ",";
  os << density( compConnCnt, wordInclCnt ) << ",";
  os << proportion( compConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_comp_conn.size(), compConnCnt ) << ",";
  os << comp_conn_mtld << ",";
  os << density( causeConnCnt, wordInclCnt ) << ",";
  os << proportion( causeConnCnt, correctedClauseCnt ) << ",";
  os << proportion( unique_cause_conn.size(), causeConnCnt ) << ",";
  os << cause_conn_mtld << ",";

  os << density( causeSitCnt, wordInclCnt ) << ",";
  os << density( spaceSitCnt, wordInclCnt ) << ",";
  os << density( timeSitCnt, wordInclCnt ) << ",";
  os << density( emoSitCnt, wordInclCnt ) << ",";
  os << proportion( unique_cause_sits.size(), causeSitCnt ) << ",";
  os << cause_sit_mtld << ",";
  os << proportion( unique_ruimte_sits.size(), spaceSitCnt ) << ",";
  os << ruimte_sit_mtld << ",";
  os << proportion( unique_tijd_sits.size(), timeSitCnt ) << ",";
  os << tijd_sit_mtld << ",";
  os << proportion( unique_emotion_sits.size(), emoSitCnt ) << ",";
  os << emotion_sit_mtld << ",";
}

void structStats::concreetHeader( ostream& os ) const {
  os << "Conc_nw_strikt_p,Conc_nw_strikt_d,";
  os << "Conc_nw_ruim_p,Conc_nw_ruim_d,";
  os << "Pers_nw_p,Pers_nw_d,";
  os << "PlantDier_nw_p,PlantDier_nw_d,";
  os << "Gebr_vw_nw_p,Gebr_vw_nw_d,"; // 20141003: Features renamed
  os << "Subst_conc_nw_p,Subst_conc_nw_d,"; // 20150508: Features renamed
  os << "Voed_verz_nw_p,Voed_verz_nw_d,"; // 20150508: Features added
  os << "Concr_ov_nw_p,Concr_ov_nw_d,";
  os << "Gebeuren_conc_nw_p,Gebeuren_conc_nw_d,"; // 20141031: Features split
  os << "Plaats_nw_p,Plaats_nw_d,";
  os << "Tijd_nw_p,Tijd_nw_d,";
  os << "Maat_nw_p,Maat_nw_d,";
  os << "Subst_abstr_nw_p,Subst_abstr_nw_d,"; // 20150508: Features added
  os << "Gebeuren_abstr_nw_p,Gebeuren_abstr_nw_d,"; // 20141031: Features split
  os << "Organisatie_nw_p,Organisatie_nw_d,";
  os << "Ov_abstr_nw_p,Ov_abstr_nw_d,"; // 20150508: Features renamed
  os << "Undefined_nw_p,";
  os << "Gedekte_nw_p,";
  os << "Alg_nw_d,Alg_nw_p,";
  os << "Alg_nw_afz_sit_d,Alg_nw_afz_sit_p,";
  os << "Alg_nw_rel_sit_d,Alg_nw_rel_sit_p,";
  os << "Alg_nw_hand_d,Alg_nw_hand_p,";
  os << "Alg_nw_kenn_d,Alg_nw_kenn_p,";
  os << "Alg_nw_disc_caus_d,Alg_nw_disc_caus_p,";
  os << "Alg_nw_ontw_d,Alg_nw_ontw_p,";
  os << "Waarn_mens_bvnw_p,Waarn_mens_bvnw_d,";
  os << "Emosoc_bvnw_p,Emosoc_bvnw_d,";
  os << "Waarn_nmens_bvnw_p,Waarn_nmens_bvnw_d,";
  os << "Vorm_omvang_bvnw_p,Vorm_omvang_bvnw_d,";
  os << "Kleur_bvnw_p,Kleur_bvnw_d,";
  os << "Stof_bvnw_p,Stof_bvnw_d,";
  os << "Geluid_bvnw_p,Geluid_bvnw_d,";
  os << "Waarn_nmens_ov_bvnw_p,Waarn_nmens_ov_bvnw_d,";
  os << "Technisch_bvnw_p,Technisch_bvnw_d,";
  os << "Tijd_bvnw_p,Tijd_bvnw_d,";
  os << "Plaats_bvnw_p,Plaats_bvnw_d,";
  os << "Spec_positief_bvnw_p,Spec_positief_bvnw_d,";
  os << "Spec_negatief_bvnw_p,Spec_negatief_bvnw_d,";
  os << "Alg_positief_bvnw_p,Alg_positief_bvnw_d,";
  os << "Alg_negatief_bvnw_p,Alg_negatief_bvnw_d,";
  os << "Alg_ev_zr_bvnw_p,Alg_ev_zr_bvnw_d,"; // 20150316: Features added
  os << "Ep_positief_bvnw_p,Ep_positief_bvnw_d,";
  os << "Ep_negatief_bvnw_p,Ep_negatief_bvnw_d,";
  os << "Ov_abstr_bvnw_p,Ov_abstr_bvnw_d,"; // 20151020: Features renamed
  os << "Spec_ev_bvnw_p,Spec_ev_bvnw_d,"; // 20150316: Features renamed
  os << "Alg_ev_bvnw_p,Alg_ev_bvnw_d,"; // 20150316: Features renamed
  os << "Ep_ev_bvnw_p,Ep_ev_bvnw_d,"; // 20150316: Features renamed
  os << "Conc_bvnw_strikt_p,Conc_bvnw_strikt_d,";
  os << "Conc_bvnw_ruim_p,Conc_bvnw_ruim_d,";
  os << "Subj_bvnw_p,Subj_bvnw_d,";
  os << "Undefined_bvnw_p,";
  os << "Gelabeld_bvnw_p,";
  os << "Gedekte_bvnw_p,";
  os << "Conc_ww_p,Conc_ww_d,";
  os << "Abstr_ww_p,Abstr_ww_d,";
  os << "Undefined_ww_p,";
  os << "Gedekte_ww_p,";
  os << "Alg_ww_d,Alg_ww_p,";
  os << "Alg_ww_afz_sit_d,Alg_ww_afz_sit_p,";
  os << "Alg_ww_rel_sit_d,Alg_ww_rel_sit_p,";
  os << "Alg_ww_hand_d,Alg_ww_hand_p,";
  os << "Alg_ww_kenn_d,Alg_ww_kenn_p,";
  os << "Alg_ww_disc_caus_d,Alg_ww_disc_caus_p,";
  os << "Alg_ww_ontw_d,Alg_ww_ontw_p,";
  os << "Conc_tot_p,Conc_tot_d,";
  os << "Alg_bijw_d,Alg_bijw_p,";
  os << "Spec_bijw_d,Spec_bijw_p,";
  os << "Gedekte_bw_p,";
}

void structStats::concreetToCSV( ostream& os ) const {
  int coveredNouns = nounCnt+nameCnt-uncoveredNounCnt;
  os << proportion( strictNounCnt, coveredNouns ) << ",";
  os << density( strictNounCnt, wordCnt ) << ",";
  os << proportion( broadNounCnt, coveredNouns ) << ",";
  os << density( broadNounCnt, wordCnt ) << ",";
  os << proportion( humanCnt, coveredNouns ) << ",";
  os << density( humanCnt, wordCnt ) << ",";
  os << proportion( nonHumanCnt, coveredNouns ) << ",";
  os << density( nonHumanCnt, wordCnt ) << ",";
  os << proportion( artefactCnt, coveredNouns ) << ",";
  os << density( artefactCnt, wordCnt ) << ",";
  os << proportion( substanceConcCnt, coveredNouns ) << ",";
  os << density( substanceConcCnt, wordCnt ) << ",";
  os << proportion( foodcareCnt, coveredNouns ) << ",";
  os << density( foodcareCnt, wordCnt ) << ",";
  os << proportion( concrotherCnt, coveredNouns ) << ",";
  os << density( concrotherCnt, wordCnt ) << ",";
  os << proportion( dynamicConcCnt, coveredNouns ) << ",";
  os << density( dynamicConcCnt, wordCnt ) << ",";
  os << proportion( placeCnt, coveredNouns ) << ",";
  os << density( placeCnt, wordCnt ) << ",";
  os << proportion( timeCnt, coveredNouns ) << ",";
  os << density( timeCnt, wordCnt ) << ",";
  os << proportion( measureCnt, coveredNouns ) << ",";
  os << density( measureCnt, wordCnt ) << ",";
  os << proportion( substanceAbstrCnt, coveredNouns ) << ",";
  os << density( substanceAbstrCnt, wordCnt ) << ",";
  os << proportion( dynamicAbstrCnt, coveredNouns ) << ",";
  os << density( dynamicAbstrCnt, wordCnt ) << ",";
  os << proportion( institutCnt, coveredNouns ) << ",";
  os << density( institutCnt, wordCnt ) << ",";
  os << proportion( nonDynamicCnt, coveredNouns ) << ",";
  os << density( nonDynamicCnt, wordCnt ) << ",";
  os << proportion( undefinedNounCnt, coveredNouns ) << ",";
  os << proportion( coveredNouns, nounCnt + nameCnt ) << ",";

  os << density( generalNounCnt, wordCnt ) << ",";
  os << proportion( generalNounCnt, coveredNouns ) << ",";
  os << density( generalNounSepCnt, wordCnt ) << ",";
  os << proportion( generalNounSepCnt, coveredNouns ) << ",";
  os << density( generalNounRelCnt, wordCnt ) << ",";
  os << proportion( generalNounRelCnt, coveredNouns ) << ",";
  os << density( generalNounActCnt, wordCnt ) << ",";
  os << proportion( generalNounActCnt, coveredNouns ) << ",";
  os << density( generalNounKnowCnt, wordCnt ) << ",";
  os << proportion( generalNounKnowCnt, coveredNouns ) << ",";
  os << density( generalNounDiscCnt, wordCnt ) << ",";
  os << proportion( generalNounDiscCnt, coveredNouns ) << ",";
  os << density( generalNounDeveCnt, wordCnt ) << ",";
  os << proportion( generalNounDeveCnt, coveredNouns ) << ",";

  int coveredAdj = adjCnt-uncoveredAdjCnt;
  os << proportion( humanAdjCnt, coveredAdj ) << ",";
  os << density( humanAdjCnt,wordCnt ) << ",";
  os << proportion( emoAdjCnt, coveredAdj ) << ",";
  os << density( emoAdjCnt,wordCnt ) << ",";
  os << proportion( nonhumanAdjCnt, coveredAdj ) << ",";
  os << density( nonhumanAdjCnt,wordCnt ) << ",";
  os << proportion( shapeAdjCnt, coveredAdj ) << ",";
  os << density( shapeAdjCnt,wordCnt ) << ",";
  os << proportion( colorAdjCnt, coveredAdj ) << ",";
  os << density( colorAdjCnt,wordCnt ) << ",";
  os << proportion( matterAdjCnt, coveredAdj ) << ",";
  os << density( matterAdjCnt,wordCnt ) << ",";
  os << proportion( soundAdjCnt, coveredAdj ) << ",";
  os << density( soundAdjCnt,wordCnt ) << ",";
  os << proportion( nonhumanOtherAdjCnt, coveredAdj ) << ",";
  os << density( nonhumanOtherAdjCnt,wordCnt ) << ",";
  os << proportion( techAdjCnt, coveredAdj ) << ",";
  os << density( techAdjCnt,wordCnt ) << ",";
  os << proportion( timeAdjCnt, coveredAdj ) << ",";
  os << density( timeAdjCnt,wordCnt ) << ",";
  os << proportion( placeAdjCnt, coveredAdj ) << ",";
  os << density( placeAdjCnt,wordCnt ) << ",";
  os << proportion( specPosAdjCnt, coveredAdj ) << ",";
  os << density( specPosAdjCnt,wordCnt ) << ",";
  os << proportion( specNegAdjCnt, coveredAdj ) << ",";
  os << density( specNegAdjCnt,wordCnt ) << ",";
  os << proportion( posAdjCnt, coveredAdj ) << ",";
  os << density( posAdjCnt,wordCnt ) << ",";
  os << proportion( negAdjCnt, coveredAdj ) << ",";
  os << density( negAdjCnt,wordCnt ) << ",";
  os << proportion( evaluativeAdjCnt, coveredAdj ) << ",";
  os << density( evaluativeAdjCnt,wordCnt ) << ",";
  os << proportion( epiPosAdjCnt, coveredAdj ) << ",";
  os << density( epiPosAdjCnt,wordCnt ) << ",";
  os << proportion( epiNegAdjCnt, coveredAdj ) << ",";
  os << density( epiNegAdjCnt,wordCnt ) << ",";
  os << proportion( abstractAdjCnt, coveredAdj ) << ",";
  os << density( abstractAdjCnt,wordCnt ) << ",";
  os << proportion( specPosAdjCnt + specNegAdjCnt, coveredAdj ) << ",";
  os << density( specPosAdjCnt + specNegAdjCnt, wordCnt ) << ",";
  os << proportion( posAdjCnt + negAdjCnt + evaluativeAdjCnt, coveredAdj ) << ",";
  os << density( posAdjCnt + negAdjCnt + evaluativeAdjCnt, wordCnt ) << ",";
  os << proportion( epiPosAdjCnt + epiNegAdjCnt, coveredAdj ) << ",";
  os << density( epiPosAdjCnt + epiNegAdjCnt ,wordCnt ) << ",";
  os << proportion( strictAdjCnt, coveredAdj ) << ",";
  os << density( strictAdjCnt, wordCnt ) << ",";
  os << proportion( broadAdjCnt, coveredAdj ) << ",";
  os << density( broadAdjCnt, wordCnt ) << ",";
  os << proportion( subjectiveAdjCnt ,coveredAdj ) << ",";
  os << density( subjectiveAdjCnt, wordCnt ) << ",";
  os << proportion( undefinedAdjCnt, coveredAdj ) << ",";
  os << proportion( coveredAdj - undefinedAdjCnt ,coveredAdj ) << ",";
  os << proportion( coveredAdj ,adjCnt ) << ",";

  int coveredVerbs = verbCnt - uncoveredVerbCnt;
  os << proportion( concreteWwCnt, coveredVerbs ) << ",";
  os << density( concreteWwCnt, wordCnt ) << ",";
  os << proportion( abstractWwCnt, coveredVerbs ) << ",";
  os << density( abstractWwCnt, wordCnt ) << ",";
  os << proportion( undefinedWwCnt, coveredVerbs ) << ",";
  os << proportion( coveredVerbs, verbCnt ) << ",";

  os << density( generalVerbCnt, wordCnt ) << ",";
  os << proportion( generalVerbCnt, coveredVerbs ) << ",";
  os << density( generalVerbSepCnt, wordCnt ) << ",";
  os << proportion( generalVerbSepCnt, coveredVerbs ) << ",";
  os << density( generalVerbRelCnt, wordCnt ) << ",";
  os << proportion( generalVerbRelCnt, coveredVerbs ) << ",";
  os << density( generalVerbActCnt, wordCnt ) << ",";
  os << proportion( generalVerbActCnt, coveredVerbs ) << ",";
  os << density( generalVerbKnowCnt, wordCnt ) << ",";
  os << proportion( generalVerbKnowCnt, coveredVerbs ) << ",";
  os << density( generalVerbDiscCnt, wordCnt ) << ",";
  os << proportion( generalVerbDiscCnt, coveredVerbs ) << ",";
  os << density( generalVerbDeveCnt, wordCnt ) << ",";
  os << proportion( generalVerbDeveCnt, coveredVerbs ) << ",";

  int totalCovered = coveredNouns + coveredAdj + coveredVerbs;
  int totalCnt = strictNounCnt + strictAdjCnt + concreteWwCnt;
  os << proportion( totalCnt, totalCovered ) << ",";
  os << density( totalCnt, wordCnt ) << ",";

  int coveredAdverbs = generalAdverbCnt + specificAdverbCnt;
  os << density( generalAdverbCnt, wordInclCnt ) << ",";
  os << proportion( generalAdverbCnt, coveredAdverbs ) << ",";
  os << density( specificAdverbCnt, wordInclCnt ) << ",";
  os << proportion( specificAdverbCnt, coveredAdverbs ) << ",";
  os << proportion( coveredAdverbs, bwCnt ) << ",";
}

void structStats::persoonlijkheidHeader( ostream& os ) const {
  os << "Pers_ref_d,Pers_vnw1_d,Pers_vnw2_d,Pers_vnw3_d,Pers_vnw_d,"
     << "Pers_namen_p, Pers_namen_p2, Pers_namen_d, Plaatsnamen_d,"
     << "Org_namen_d, Prod_namen_d, Event_namen_d,";
}

void structStats::persoonlijkheidToCSV( ostream& os ) const {
  os << density( persRefCnt, wordInclCnt ) << ",";
  os << density( pron1Cnt, wordInclCnt ) << ",";
  os << density( pron2Cnt, wordInclCnt ) << ",";
  os << density( pron3Cnt, wordInclCnt ) << ",";
  os << density( pron1Cnt+pron2Cnt+pron3Cnt, wordInclCnt ) << ",";

  int val = at( ners, NER::PER_B );
  os << proportion( val, nerCnt ) << ",";
  os << proportion( val, nounCnt + nameCnt ) << ",";
  os << density( val, wordCnt ) << ",";
  val = at( ners, NER::LOC_B );
  os << density( val, wordCnt ) << ",";
  val = at( ners, NER::ORG_B );
  os << density( val, wordCnt ) << ",";
  val = at( ners, NER::PRO_B );
  os << density( val, wordCnt ) << ",";
  val = at( ners, NER::EVE_B );
  os << density( val, wordCnt ) << ",";
}

void structStats::verbHeader( ostream& os ) const {
  os << "Actieww_p,Actieww_d,Toestww_p,Toestww_d,"
     << "Procesww_p,Procesww_d,Undefined_ATP_ww_p,"
     << "Ww_tt_p,Ww_tt_dz,Ww_mod_d_,Ww_mod_dz,"
     << "Huww_tijd_d,Huww_tijd_dz,Koppelww_d,Koppelww_dz,"
     << "Infin_bv_d,Infin_bv_dz,"
     << "Infin_nw_d,Infin_nw_dz,"
     << "Infin_vrij_d,Infin_vrij_dz,"
     << "Vd_bv_d,Vd_bv_dz,"
     << "Vd_nw_d,Vd_nw_dz,"
     << "Vd_vrij_d,Vd_vrij_dz,"
     << "Ovd_bv_d,Ovd_bv_dz,"
     << "Ovd_nw_d,Ovd_nw_dz,"
     << "Ovd_vrij_d,Ovd_vrij_dz,";
}

void structStats::verbToCSV( ostream& os ) const {
  os << proportion( actionCnt, verbCnt ) << ",";
  os << density( actionCnt, wordCnt) << ",";
  os << proportion( stateCnt, verbCnt ) << ",";
  os << density( stateCnt, wordCnt ) << ",";
  os << proportion( processCnt, verbCnt ) << ",";
  os << density( processCnt, wordCnt ) << ",";
  os << proportion( undefinedATPCnt, verbCnt - uncoveredVerbCnt ) << ",";

  os << density( presentCnt, wordInclCnt ) << ",";
  os << proportion( presentCnt, correctedClauseCnt ) << ",";
  os << density( modalCnt, wordInclCnt ) << ",";
  os << proportion( modalCnt, correctedClauseCnt ) << ",";
  os << density( timeVCnt, wordInclCnt ) << ",";
  os << proportion( timeVCnt, correctedClauseCnt ) << ",";
  os << density( koppelCnt, wordInclCnt ) << ",";
  os << proportion( koppelCnt, correctedClauseCnt ) << ",";

  os << density( infBvCnt, wordInclCnt ) << ",";
  os << proportion( infBvCnt, correctedClauseCnt ) << ",";
  os << density( infNwCnt, wordInclCnt ) << ",";
  os << proportion( infNwCnt, correctedClauseCnt ) << ",";
  os << density( infVrijCnt, wordInclCnt ) << ",";
  os << proportion( infVrijCnt, correctedClauseCnt ) << ",";

  os << density( vdBvCnt, wordInclCnt ) << ",";
  os << proportion( vdBvCnt, correctedClauseCnt ) << ",";
  os << density( vdNwCnt, wordInclCnt ) << ",";
  os << proportion( vdNwCnt, correctedClauseCnt ) << ",";
  os << density( vdVrijCnt, wordInclCnt ) << ",";
  os << proportion( vdVrijCnt, correctedClauseCnt ) << ",";

  os << density( odBvCnt, wordInclCnt ) << ",";
  os << proportion( odBvCnt, correctedClauseCnt ) << ",";
  os << density( odNwCnt, wordInclCnt ) << ",";
  os << proportion( odNwCnt, correctedClauseCnt ) << ",";
  os << density( odVrijCnt, wordInclCnt ) << ",";
  os << proportion( odVrijCnt, correctedClauseCnt ) << ",";
}

void structStats::imperativeHeader( ostream& os ) const {
  os << "Imp_ellips_p,Imp_ellips_d," // 20141003: Features renamed
     << "Vragen_p,Vragen_d,";
}

void structStats::imperativeToCSV( ostream& os ) const {
  os << proportion( impCnt, sentCnt ) << ",";
  os << density( impCnt, wordInclCnt ) << ",";
  os << proportion( questCnt, sentCnt ) << ",";
  os << density( questCnt, wordInclCnt ) << ",";
}

void structStats::wordSortHeader( ostream& os ) const {
  os << "Bvnw_d,Vg_d,Vnw_d,Lidw_d,Vz_d,Bijw_d,Tw_d,Nw_d,Ww_d,Tuss_d,Spec_d,"
     << "Interp_d,"
     << "Afk_d,Afk_gen_d,Afk_int_d,Afk_jur_d,Afk_med_d,"
     << "Afk_ond_d,Afk_pol_d,Afk_ov_d,Afk_zorg_d,";
}

void structStats::wordSortToCSV( ostream& os ) const {
  os << density(adjInclCnt, wordInclCnt ) << ","
     << density(vgCnt, wordInclCnt ) << ","
     << density(vnwCnt, wordInclCnt ) << ","
     << density(lidCnt, wordInclCnt ) << ","
     << density(vzCnt, wordInclCnt ) << ","
     << density(bwCnt, wordInclCnt ) << ","
     << density(twCnt, wordInclCnt ) << ","
     << density(nounInclCnt, wordInclCnt ) << ","
     << density(verbInclCnt, wordInclCnt ) << ","
     << density(tswCnt, wordInclCnt ) << ","
     << density(specCnt, wordInclCnt ) << ","
     << density(letCnt, wordInclCnt ) << ",";
  int pola = at( afks, Afk::OVERHEID_A );
  int jura = at( afks, Afk::JURIDISCH_A );
  int onda = at( afks, Afk::ONDERWIJS_A );
  int meda = at( afks, Afk::MEDIA_A );
  int gena = at( afks, Afk::GENERIEK_A );
  int ova = at( afks, Afk::OVERIGE_A );
  int zorga = at( afks, Afk::ZORG_A );
  int inta = at( afks, Afk::INTERNATIONAAL_A );
  os << density( gena+inta+jura+meda+onda+pola+ova+zorga, wordInclCnt ) << ","
     << density( gena, wordInclCnt ) << ","
     << density( inta, wordInclCnt ) << ","
     << density( jura, wordInclCnt ) << ","
     << density( meda, wordInclCnt ) << ","
     << density( onda, wordInclCnt ) << ","
     << density( pola, wordInclCnt ) << ","
     << density( ova, wordInclCnt ) << ","
     << density( zorga, wordInclCnt ) << ",";
}

void structStats::prepPhraseHeader( ostream& os ) const {
  os << "Vzu_d,Vzu_dz,Arch_d,";
}

void structStats::prepPhraseToCSV( ostream& os ) const {
  os << density( prepExprCnt, wordInclCnt ) << ",";
  os << proportion( prepExprCnt, correctedClauseCnt ) << ",";
  os << density( archaicsCnt, wordInclCnt ) << ",";
}

void structStats::intensHeader( ostream& os ) const {
  os << "Int_d,Int_bvnw_d,Int_bvbw_d,";
  os << "Int_bw_d,Int_combi_d,Int_nw_d,";
  os << "Int_tuss_d,Int_ww_d,";
}

void structStats::intensToCSV( ostream& os ) const {
  os << density( intensCnt, wordInclCnt ) << ",";
  os << density( intensBvnwCnt, wordInclCnt ) << ",";
  os << density( intensBvbwCnt, wordInclCnt ) << ",";
  os << density( intensBwCnt, wordInclCnt ) << ",";
  os << density( intensCombiCnt, wordInclCnt ) << ",";
  os << density( intensNwCnt, wordInclCnt ) << ",";
  os << density( intensTussCnt, wordInclCnt ) << ",";
  os << density( intensWwCnt, wordInclCnt ) << ",";
}

void structStats::miscHeader( ostream& os ) const {
  os << "Log_prob_fwd,Log_prob_fwd_inhwrd,Log_prob_fwd_zn,Log_prob_fwd_inhwrd_zn,";
  os << "Entropie_fwd,Entropie_fwd_norm,Perplexiteit_fwd,Perplexiteit_fwd_norm,";
  os << "Log_prob_bwd,Log_prob_bwd_inhwrd,Log_prob_bwd_zn,Log_prob_bwd_inhwrd_zn,";
  os << "Entropie_bwd,Entropie_bwd_norm,Perplexiteit_bwd,Perplexiteit_bwd_norm,";
  os << "Eigen_classificatie,";
  os << "LiNT_score1,LiNT_score2";
}

void structStats::miscToCSV( ostream& os ) const {
  os << proportion( avg_prob10_fwd, sentCnt ) << ",";
  os << proportion( avg_prob10_fwd_content, sentCnt ) << ",";
  os << proportion( avg_prob10_fwd_ex_names, sentCnt ) << ",";
  os << proportion( avg_prob10_fwd_content_ex_names, sentCnt ) << ",";
  os << proportion( entropy_fwd, sentCnt ) << ",";
  os << proportion( entropy_fwd_norm, sentCnt ) << ",";
  os << proportion( perplexity_fwd, sentCnt ) << ",";
  os << proportion( perplexity_fwd_norm, sentCnt ) << ",";
  os << proportion( avg_prob10_bwd, sentCnt ) << ",";
  os << proportion( avg_prob10_bwd_content, sentCnt ) << ",";
  os << proportion( avg_prob10_bwd_ex_names, sentCnt ) << ",";
  os << proportion( avg_prob10_bwd_content_ex_names, sentCnt ) << ",";
  os << proportion( entropy_bwd, sentCnt ) << ",";
  os << proportion( entropy_bwd_norm, sentCnt ) << ",";
  os << proportion( perplexity_bwd, sentCnt ) << ",";
  os << proportion( perplexity_bwd_norm, sentCnt ) << ",";

  os << "\"" << escape_quotes(toStringCounter(my_classification)) << "\",";

  /* LINT scores */
  double wrd_freq_log_zn_corr = proportion(word_freq_log_n_corr_strict, contentStrictCnt-nameCnt).p;
  double bijv_bep_dz_zbijzin = proportion( max(0, npModCnt - betrCnt), correctedClauseCnt).p ;
  double alg_nw_d = density( generalNounCnt, wordCnt ).d;
  double Inhwrd_dz_zonder_abw = proportion( contentStrictInclCnt, correctedClauseCnt ).p;
  double Conc_nw_ruim_p = proportion( broadNounCnt, nounCnt+nameCnt-uncoveredNounCnt ).p;

  double lint_score_1 = -14.857
    +  19.487 * wrd_freq_log_zn_corr 
    - 5.965 * bijv_bep_dz_zbijzin 
    - 0.093 * alg_nw_d 
    - 0.995 * al_max ;
  double lint_score_2 = -9.925 
    + 18.264 * wrd_freq_log_zn_corr 
    - 3.766 * Inhwrd_dz_zonder_abw 
    + 13.796 * Conc_nw_ruim_p 
    - 1.126 * al_max;

  os << lint_score_1 << ",";
  os << lint_score_2 << ",";
}

/**************
 * FOLIA OUTPUT
 **************/

/**
 * Add Metrics to a FoLiA Document.
 */
void structStats::addMetrics( ) const {
  folia::FoliaElement *el = folia_node;
  folia::Document *doc = el->doc();
  addOneMetric( doc, el, "word_count", TiCC::toString(wordCnt) );
  addOneMetric( doc, el, "word_count_incl_stopwords", TiCC::toString(wordInclCnt) );
  addOneMetric( doc, el, "bv_vd_count", TiCC::toString(vdBvCnt) );
  addOneMetric( doc, el, "nw_vd_count", TiCC::toString(vdNwCnt) );
  addOneMetric( doc, el, "vrij_vd_count", TiCC::toString(vdVrijCnt) );
  addOneMetric( doc, el, "bv_od_count", TiCC::toString(odBvCnt) );
  addOneMetric( doc, el, "nw_od_count", TiCC::toString(odNwCnt) );
  addOneMetric( doc, el, "vrij_od_count", TiCC::toString(odVrijCnt) );
  addOneMetric( doc, el, "bv_inf_count", TiCC::toString(infBvCnt) );
  addOneMetric( doc, el, "nw_inf_count", TiCC::toString(infNwCnt) );
  addOneMetric( doc, el, "vrij_inf_count", TiCC::toString(infVrijCnt) );
  addOneMetric( doc, el, "smain_count", TiCC::toString(smainCnt) );
  addOneMetric( doc, el, "ssub_count", TiCC::toString(ssubCnt) );
  addOneMetric( doc, el, "sv1_count", TiCC::toString(sv1Cnt) );
  addOneMetric( doc, el, "smain_cnj_count", TiCC::toString(smainCnjCnt) );
  addOneMetric( doc, el, "ssub_cnj_count", TiCC::toString(ssubCnjCnt) );
  addOneMetric( doc, el, "sv1_cnj_count", TiCC::toString(sv1CnjCnt) );
  addOneMetric( doc, el, "present_verb_count", TiCC::toString(presentCnt) );
  addOneMetric( doc, el, "past_verb_count", TiCC::toString(pastCnt) );
  addOneMetric( doc, el, "subjonct_count", TiCC::toString(subjonctCnt) );
  addOneMetric( doc, el, "name_count", TiCC::toString(nameCnt) );
  int val = at( ners, NER::PER_B );
  addOneMetric( doc, el, "personal_name_count", TiCC::toString(val) );
  val = at( ners, NER::LOC_B );
  addOneMetric( doc, el, "location_name_count", TiCC::toString(val) );
  val = at( ners, NER::ORG_B );
  addOneMetric( doc, el, "organization_name_count", TiCC::toString(val) );
  val = at( ners, NER::PRO_B );
  addOneMetric( doc, el, "product_name_count", TiCC::toString(val) );
  val = at( ners, NER::EVE_B );
  addOneMetric( doc, el, "event_name_count", TiCC::toString(val) );
  val = at( afks, Afk::OVERHEID_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "overheid_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::JURIDISCH_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "juridisch_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::ONDERWIJS_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "onderwijs_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::MEDIA_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "media_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::GENERIEK_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "generiek_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::OVERIGE_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "overige_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::INTERNATIONAAL_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "internationaal_afk_count", TiCC::toString(val) );
  }
  val = at( afks, Afk::ZORG_A );
  if ( val > 0 ){
    addOneMetric( doc, el, "zorg_afk_count", TiCC::toString(val) );
  }

  addOneMetric( doc, el, "pers_pron_1_count", TiCC::toString(pron1Cnt) );
  addOneMetric( doc, el, "pers_pron_2_count", TiCC::toString(pron2Cnt) );
  addOneMetric( doc, el, "pers_pron_3_count", TiCC::toString(pron3Cnt) );
  addOneMetric( doc, el, "passive_count", TiCC::toString(passiveCnt) );
  addOneMetric( doc, el, "modal_count", TiCC::toString(modalCnt) );
  addOneMetric( doc, el, "time_count", TiCC::toString(timeVCnt) );
  addOneMetric( doc, el, "koppel_count", TiCC::toString(koppelCnt) );
  addOneMetric( doc, el, "pers_ref_count", TiCC::toString(persRefCnt) );
  addOneMetric( doc, el, "pron_ref_count", TiCC::toString(pronRefCnt) );
  addOneMetric( doc, el, "archaic_count", TiCC::toString(archaicsCnt) );
  addOneMetric( doc, el, "content_count", TiCC::toString(contentCnt) );
  addOneMetric( doc, el, "content_strict_count", TiCC::toString(contentStrictCnt) );
  addOneMetric( doc, el, "nominal_count", TiCC::toString(nominalCnt) );
  addOneMetric( doc, el, "adj_count", TiCC::toString(adjCnt) );
  addOneMetric( doc, el, "vg_count", TiCC::toString(vgCnt) );
  addOneMetric( doc, el, "vnw_count", TiCC::toString(vnwCnt) );
  addOneMetric( doc, el, "lid_count", TiCC::toString(lidCnt) );
  addOneMetric( doc, el, "vz_count", TiCC::toString(vzCnt) );
  addOneMetric( doc, el, "bw_count", TiCC::toString(bwCnt) );
  addOneMetric( doc, el, "tw_count", TiCC::toString(twCnt) );
  addOneMetric( doc, el, "noun_count", TiCC::toString(nounCnt) );
  addOneMetric( doc, el, "verb_count", TiCC::toString(verbCnt) );
  addOneMetric( doc, el, "tsw_count", TiCC::toString(tswCnt) );
  addOneMetric( doc, el, "spec_count", TiCC::toString(specCnt) );
  addOneMetric( doc, el, "let_count", TiCC::toString(letCnt) );
  addOneMetric( doc, el, "rel_count", TiCC::toString(betrCnt) );
  addOneMetric( doc, el, "all_connector_count", TiCC::toString(allConnCnt) );
  addOneMetric( doc, el, "temporal_connector_count", TiCC::toString(tempConnCnt) );
  addOneMetric( doc, el, "reeks_wg_connector_count", TiCC::toString(opsomWgConnCnt) );
  addOneMetric( doc, el, "reeks_zin_connector_count", TiCC::toString(opsomZinConnCnt) );
  addOneMetric( doc, el, "contrast_connector_count", TiCC::toString(contrastConnCnt) );
  addOneMetric( doc, el, "comparatief_connector_count", TiCC::toString(compConnCnt) );
  addOneMetric( doc, el, "causaal_connector_count", TiCC::toString(causeConnCnt) );
  addOneMetric( doc, el, "time_situation_count", TiCC::toString(timeSitCnt) );
  addOneMetric( doc, el, "space_situation_count", TiCC::toString(spaceSitCnt) );
  addOneMetric( doc, el, "cause_situation_count", TiCC::toString(causeSitCnt) );
  addOneMetric( doc, el, "emotion_situation_count", TiCC::toString(emoSitCnt) );
  addOneMetric( doc, el, "prop_neg_count", TiCC::toString(propNegCnt) );
  addOneMetric( doc, el, "morph_neg_count", TiCC::toString(morphNegCnt) );
  addOneMetric( doc, el, "multiple_neg_count", TiCC::toString(multiNegCnt) );
  addOneMetric( doc, el, "voorzetsel_expression_count", TiCC::toString(prepExprCnt) );
  addOneMetric( doc, el,
    "word_overlap_count", TiCC::toString( wordOverlapCnt ) );
  addOneMetric( doc, el,
    "lemma_overlap_count", TiCC::toString( lemmaOverlapCnt ) );
  addOneMetric( doc, el, "prevalenceP", TiCC::toString(prevalenceP) );
  addOneMetric( doc, el, "prevalenceZ", TiCC::toString(prevalenceZ) );
  addOneMetric( doc, el, "prevalenceContentP", TiCC::toString(prevalenceContentP) );
  addOneMetric( doc, el, "prevalenceContentZ", TiCC::toString(prevalenceContentZ) );
  addOneMetric( doc, el, "prevalenceCovered", TiCC::toString(prevalenceCovered) );
  addOneMetric( doc, el, "prevalenceContentCovered", TiCC::toString(prevalenceContentCovered) );
  addOneMetric( doc, el, "freq50", TiCC::toString(f50Cnt) );
  addOneMetric( doc, el, "freq65", TiCC::toString(f65Cnt) );
  addOneMetric( doc, el, "freq77", TiCC::toString(f77Cnt) );
  addOneMetric( doc, el, "freq80", TiCC::toString(f80Cnt) );

  addOneMetric( doc, el, "top1000", TiCC::toString(top1000Cnt) );
  addOneMetric( doc, el, "top2000", TiCC::toString(top2000Cnt) );
  addOneMetric( doc, el, "top3000", TiCC::toString(top3000Cnt) );
  addOneMetric( doc, el, "top5000", TiCC::toString(top5000Cnt) );
  addOneMetric( doc, el, "top10000", TiCC::toString(top10000Cnt) );
  addOneMetric( doc, el, "top20000", TiCC::toString(top20000Cnt) );
  addOneMetric( doc, el, "top1000Content", TiCC::toString(top1000ContentCnt) );
  addOneMetric( doc, el, "top2000Content", TiCC::toString(top2000ContentCnt) );
  addOneMetric( doc, el, "top3000Content", TiCC::toString(top3000ContentCnt) );
  addOneMetric( doc, el, "top5000Content", TiCC::toString(top5000ContentCnt) );
  addOneMetric( doc, el, "top10000Content", TiCC::toString(top10000ContentCnt) );
  addOneMetric( doc, el, "top20000Content", TiCC::toString(top20000ContentCnt) );
  addOneMetric( doc, el, "top1000StrictContent", TiCC::toString(top1000ContentStrictCnt) );
  addOneMetric( doc, el, "top2000StrictContent", TiCC::toString(top2000ContentStrictCnt) );
  addOneMetric( doc, el, "top3000StrictContent", TiCC::toString(top3000ContentStrictCnt) );
  addOneMetric( doc, el, "top5000StrictContent", TiCC::toString(top5000ContentStrictCnt) );
  addOneMetric( doc, el, "top10000StrictContent", TiCC::toString(top10000ContentStrictCnt) );
  addOneMetric( doc, el, "top20000StrictContent", TiCC::toString(top20000ContentStrictCnt) );

  addOneMetric( doc, el, "word_freq", TiCC::toString(word_freq) );
  addOneMetric( doc, el, "word_freq_no_names", TiCC::toString(word_freq_n) );
  if ( !std::isnan(word_freq_log)  )
    addOneMetric( doc, el, "log_word_freq", TiCC::toString(word_freq_log) );
  if ( !std::isnan(word_freq_log_n)  )
    addOneMetric( doc, el, "log_word_freq_no_names", TiCC::toString(word_freq_log_n) );
  addOneMetric( doc, el, "lemma_freq", TiCC::toString(lemma_freq) );
  addOneMetric( doc, el, "lemma_freq_no_names", TiCC::toString(lemma_freq_n) );
  if ( !std::isnan(lemma_freq_log)  )
    addOneMetric( doc, el, "log_lemma_freq", TiCC::toString(lemma_freq_log) );
  if ( !std::isnan(lemma_freq_log_n)  )
    addOneMetric( doc, el, "log_lemma_freq_no_names", TiCC::toString(lemma_freq_log_n) );

  if ( !std::isnan(word_freq_log_strict)  )
    addOneMetric( doc, el, "log_word_freq_strict", TiCC::toString(word_freq_log_strict) );
  if ( !std::isnan(word_freq_log_n_strict)  )
    addOneMetric( doc, el, "log_word_freq_no_names_strict", TiCC::toString(word_freq_log_n_strict) );
  if ( !std::isnan(lemma_freq_log_strict)  )
    addOneMetric( doc, el, "log_lemma_freq_strict", TiCC::toString(lemma_freq_log_strict) );
  if ( !std::isnan(lemma_freq_log_n_strict)  )
    addOneMetric( doc, el, "log_lemma_freq_no_names_strict", TiCC::toString(lemma_freq_log_n_strict) );

  if ( !std::isnan(avg_prob10_fwd) )
    addOneMetric( doc, el, "wopr_logprob_fwd", TiCC::toString(avg_prob10_fwd) );
  if ( !std::isnan(entropy_fwd) )
    addOneMetric( doc, el, "wopr_entropy_fwd", TiCC::toString(entropy_fwd) );
  if ( !std::isnan(perplexity_fwd) )
    addOneMetric( doc, el, "wopr_perplexity_fwd", TiCC::toString(perplexity_fwd) );
  if ( !std::isnan(avg_prob10_bwd) )
    addOneMetric( doc, el, "wopr_logprob_bwd", TiCC::toString(avg_prob10_bwd) );
  if ( !std::isnan(entropy_bwd) )
    addOneMetric( doc, el, "wopr_entropy_bwd", TiCC::toString(entropy_bwd) );
  if ( !std::isnan(perplexity_bwd) )
    addOneMetric( doc, el, "wopr_perplexity_bwd", TiCC::toString(perplexity_bwd) );

  addOneMetric( doc, el, "broad_adj", TiCC::toString(broadAdjCnt) );
  addOneMetric( doc, el, "strict_adj", TiCC::toString(strictAdjCnt) );
  addOneMetric( doc, el, "human_adj_count", TiCC::toString(humanAdjCnt) );
  addOneMetric( doc, el, "emo_adj_count", TiCC::toString(emoAdjCnt) );
  addOneMetric( doc, el, "nonhuman_adj_count", TiCC::toString(nonhumanAdjCnt) );
  addOneMetric( doc, el, "shape_adj_count", TiCC::toString(shapeAdjCnt) );
  addOneMetric( doc, el, "color_adj_count", TiCC::toString(colorAdjCnt) );
  addOneMetric( doc, el, "matter_adj_count", TiCC::toString(matterAdjCnt) );
  addOneMetric( doc, el, "sound_adj_count", TiCC::toString(soundAdjCnt) );
  addOneMetric( doc, el, "other_nonhuman_adj_count", TiCC::toString(nonhumanOtherAdjCnt) );
  addOneMetric( doc, el, "techn_adj_count", TiCC::toString(techAdjCnt) );
  addOneMetric( doc, el, "time_adj_count", TiCC::toString(timeAdjCnt) );
  addOneMetric( doc, el, "place_adj_count", TiCC::toString(placeAdjCnt) );
  addOneMetric( doc, el, "pos_spec_adj_count", TiCC::toString(specPosAdjCnt) );
  addOneMetric( doc, el, "neg_spec_adj_count", TiCC::toString(specNegAdjCnt) );
  addOneMetric( doc, el, "pos_adj_count", TiCC::toString(posAdjCnt) );
  addOneMetric( doc, el, "neg_adj_count", TiCC::toString(negAdjCnt) );
  addOneMetric( doc, el, "evaluative_adj_count", TiCC::toString(evaluativeAdjCnt) );
  addOneMetric( doc, el, "pos_epi_adj_count", TiCC::toString(epiPosAdjCnt) );
  addOneMetric( doc, el, "neg_epi_adj_count", TiCC::toString(epiNegAdjCnt) );
  addOneMetric( doc, el, "abstract_adj", TiCC::toString(abstractAdjCnt) );
  addOneMetric( doc, el, "undefined_adj_count", TiCC::toString(undefinedAdjCnt) );
  addOneMetric( doc, el, "covered_adj_count", TiCC::toString(adjCnt-uncoveredAdjCnt) );
  addOneMetric( doc, el, "uncovered_adj_count", TiCC::toString(uncoveredAdjCnt) );

  addOneMetric( doc, el, "intens_count", TiCC::toString(intensCnt) );
  addOneMetric( doc, el, "intens_bvnw_count", TiCC::toString(intensBvnwCnt) );
  addOneMetric( doc, el, "intens_bvbw_count", TiCC::toString(intensBvbwCnt) );
  addOneMetric( doc, el, "intens_bw_count", TiCC::toString(intensBwCnt) );
  addOneMetric( doc, el, "intens_combi_count", TiCC::toString(intensCombiCnt) );
  addOneMetric( doc, el, "intens_nw_count", TiCC::toString(intensNwCnt) );
  addOneMetric( doc, el, "intens_tuss_count", TiCC::toString(intensTussCnt) );
  addOneMetric( doc, el, "intens_ww_count", TiCC::toString(intensWwCnt) );

  addOneMetric( doc, el, "general_noun_count", TiCC::toString(generalNounCnt) );
  addOneMetric( doc, el, "general_noun_sep_count", TiCC::toString(generalNounSepCnt) );
  addOneMetric( doc, el, "general_noun_rel_count", TiCC::toString(generalNounRelCnt) );
  addOneMetric( doc, el, "general_noun_act_count", TiCC::toString(generalNounActCnt) );
  addOneMetric( doc, el, "general_noun_know_count", TiCC::toString(generalNounKnowCnt) );
  addOneMetric( doc, el, "general_noun_disc_count", TiCC::toString(generalNounDiscCnt) );
  addOneMetric( doc, el, "general_noun_deve_count", TiCC::toString(generalNounDeveCnt) );

  addOneMetric( doc, el, "general_verb_count", TiCC::toString(generalVerbCnt) );
  addOneMetric( doc, el, "general_verb_sep_count", TiCC::toString(generalVerbSepCnt) );
  addOneMetric( doc, el, "general_verb_rel_count", TiCC::toString(generalVerbRelCnt) );
  addOneMetric( doc, el, "general_verb_act_count", TiCC::toString(generalVerbActCnt) );
  addOneMetric( doc, el, "general_verb_know_count", TiCC::toString(generalVerbKnowCnt) );
  addOneMetric( doc, el, "general_verb_disc_count", TiCC::toString(generalVerbDiscCnt) );
  addOneMetric( doc, el, "general_verb_deve_count", TiCC::toString(generalVerbDeveCnt) );

  addOneMetric( doc, el, "general_adverb_count", TiCC::toString(generalAdverbCnt) );
  addOneMetric( doc, el, "specific_adverb_count", TiCC::toString(specificAdverbCnt) );

  addOneMetric( doc, el, "broad_noun", TiCC::toString(broadNounCnt) );
  addOneMetric( doc, el, "strict_noun", TiCC::toString(strictNounCnt) );
  addOneMetric( doc, el, "human_nouns_count", TiCC::toString(humanCnt) );
  addOneMetric( doc, el, "nonhuman_nouns_count", TiCC::toString(nonHumanCnt) );
  addOneMetric( doc, el, "artefact_nouns_count", TiCC::toString(artefactCnt) );
  addOneMetric( doc, el, "concrother_nouns_count", TiCC::toString(concrotherCnt) );
  addOneMetric( doc, el, "substance_conc_nouns_count", TiCC::toString(substanceConcCnt) );
  addOneMetric( doc, el, "foodcare_nouns_count", TiCC::toString(foodcareCnt) );
  addOneMetric( doc, el, "time_nouns_count", TiCC::toString(timeCnt) );
  addOneMetric( doc, el, "place_nouns_count", TiCC::toString(placeCnt) );
  addOneMetric( doc, el, "measure_nouns_count", TiCC::toString(measureCnt) );
  addOneMetric( doc, el, "dynamic_conc_nouns_count", TiCC::toString(dynamicConcCnt) );
  addOneMetric( doc, el, "substance_abstr_nouns_count", TiCC::toString(substanceAbstrCnt) );
  addOneMetric( doc, el, "dynamic_abstr_nouns_count", TiCC::toString(dynamicAbstrCnt) );
  addOneMetric( doc, el, "nondynamic_nouns_count", TiCC::toString(nonDynamicCnt) );
  addOneMetric( doc, el, "institut_nouns_count", TiCC::toString(institutCnt) );
  addOneMetric( doc, el, "undefined_nouns_count", TiCC::toString(undefinedNounCnt) );
  addOneMetric( doc, el, "covered_nouns_count", TiCC::toString(nounCnt+nameCnt-uncoveredNounCnt) );
  addOneMetric( doc, el, "uncovered_nouns_count", TiCC::toString(uncoveredNounCnt) );

  addOneMetric( doc, el, "abstract_ww", TiCC::toString(abstractWwCnt) );
  addOneMetric( doc, el, "concrete_ww", TiCC::toString(concreteWwCnt) );
  addOneMetric( doc, el, "undefined_ww", TiCC::toString(undefinedWwCnt) );
  addOneMetric( doc, el, "undefined_ATP", TiCC::toString(undefinedATPCnt) );
  addOneMetric( doc, el, "state_count", TiCC::toString(stateCnt) );
  addOneMetric( doc, el, "action_count", TiCC::toString(actionCnt) );
  addOneMetric( doc, el, "process_count", TiCC::toString(processCnt) );
  addOneMetric( doc, el, "covered_verb_count", TiCC::toString(verbCnt-uncoveredVerbCnt) );
  addOneMetric( doc, el, "uncovered_verb_count", TiCC::toString(uncoveredVerbCnt) );
  addOneMetric( doc, el, "indef_np_count", TiCC::toString(indefNpCnt) );
  addOneMetric( doc, el, "np_count", TiCC::toString(npCnt) );
  addOneMetric( doc, el, "np_size", TiCC::toString(npSize) );
  addOneMetric( doc, el, "vc_modifier_count", TiCC::toString(vcModCnt) );
  addOneMetric( doc, el, "vc_modifier_single_count", TiCC::toString(vcModSingleCnt) );
  addOneMetric( doc, el, "adj_np_modifier_count", TiCC::toString(adjNpModCnt) );
  addOneMetric( doc, el, "np_modifier_count", TiCC::toString(npModCnt) );

  addOneMetric( doc, el, "character_count", TiCC::toString(charCnt) );
  addOneMetric( doc, el, "character_count_min_names", TiCC::toString(charCntExNames) );
  addOneMetric( doc, el, "morpheme_count", TiCC::toString(morphCnt) );
  addOneMetric( doc, el, "morpheme_count_min_names", TiCC::toString(morphCntExNames) );
  if ( dLevel >= 0 )
    addOneMetric( doc, el, "d_level", TiCC::toString(dLevel) );
  else
    addOneMetric( doc, el, "d_level", "missing" );
  if ( dLevel_gt4 != 0 )
    addOneMetric( doc, el, "d_level_gt4", TiCC::toString(dLevel_gt4) );
  if ( questCnt > 0 )
    addOneMetric( doc, el, "question_count", TiCC::toString(questCnt) );
  if ( impCnt > 0 )
    addOneMetric( doc, el, "imperative_count", TiCC::toString(impCnt) );
  addOneMetric( doc, el, "sub_verb_dist", MMtoString( distances, SUB_VERB ) );
  addOneMetric( doc, el, "obj_verb_dist", MMtoString( distances, OBJ1_VERB ) );
  addOneMetric( doc, el, "lijdend_verb_dist", MMtoString( distances, OBJ2_VERB ) );
  addOneMetric( doc, el, "verb_pp_dist", MMtoString( distances, VERB_PP ) );
  addOneMetric( doc, el, "noun_det_dist", MMtoString( distances, NOUN_DET ) );
  addOneMetric( doc, el, "prep_obj_dist", MMtoString( distances, PREP_OBJ1 ) );
  addOneMetric( doc, el, "verb_vc_dist", MMtoString( distances, VERB_VC ) );
  addOneMetric( doc, el, "comp_body_dist", MMtoString( distances, COMP_BODY ) );
  addOneMetric( doc, el, "crd_cnj_dist", MMtoString( distances, CRD_CNJ ) );
  addOneMetric( doc, el, "verb_comp_dist", MMtoString( distances, VERB_COMP ) );
  addOneMetric( doc, el, "noun_vc_dist", MMtoString( distances, NOUN_VC ) );
  addOneMetric( doc, el, "verb_svp_dist", MMtoString( distances, VERB_SVP ) );
  addOneMetric( doc, el, "verb_cop_dist", MMtoString( distances, VERB_PREDC_N ) );
  addOneMetric( doc, el, "verb_adj_dist", MMtoString( distances, VERB_PREDC_A ) );
  addOneMetric( doc, el, "verb_bw_mod_dist", MMtoString( distances, VERB_MOD_BW ) );
  addOneMetric( doc, el, "verb_adv_mod_dist", MMtoString( distances, VERB_MOD_A ) );
  addOneMetric( doc, el, "verb_noun_dist", MMtoString( distances, VERB_NOUN ) );

  if ( !my_classification.empty() )
    addOneMetric( doc, el, "my_classification", toStringCounter(my_classification) );

  addOneMetric( doc, el, "deplen", toMString( al_gem ) );
  addOneMetric( doc, el, "max_deplen", toMString( al_max ) );
  for ( size_t i=0; i < sv.size(); ++i ){
    sv[i]->addMetrics();
  }
}

/*******
 * MERGE
 *******/

void structStats::merge( structStats *ss ){
  if ( ss->parseFailCnt == -1 ) // not parsed
    parseFailCnt = -1;
  else
    parseFailCnt += ss->parseFailCnt;
  wordCnt += ss->wordCnt;
  wordInclCnt += ss->wordInclCnt;
  if ( ss->wordCnt != 0 ) // don't count sentences without words
    sentCnt += ss->sentCnt;
  charCnt += ss->charCnt;
  charCntExNames += ss->charCntExNames;
  morphCnt += ss->morphCnt;
  morphCntExNames += ss->morphCntExNames;
  nameCnt += ss->nameCnt;
  nameInclCnt += ss->nameInclCnt;
  infBvCnt += ss->infBvCnt;
  infNwCnt += ss->infNwCnt;
  infVrijCnt += ss->infVrijCnt;
  vdBvCnt += ss->vdBvCnt;
  vdNwCnt += ss->vdNwCnt;
  vdVrijCnt += ss->vdVrijCnt;
  odBvCnt += ss->odBvCnt;
  odNwCnt += ss->odNwCnt;
  odVrijCnt += ss->odVrijCnt;
  passiveCnt += ss->passiveCnt;
  modalCnt += ss->modalCnt;
  timeVCnt += ss->timeVCnt;
  koppelCnt += ss->koppelCnt;
  archaicsCnt += ss->archaicsCnt;
  contentCnt += ss->contentCnt;
  contentInclCnt += ss->contentInclCnt;
  contentStrictCnt += ss->contentStrictCnt;
  contentStrictInclCnt += ss->contentStrictInclCnt;
  nominalCnt += ss->nominalCnt;
  adjCnt += ss->adjCnt;
  adjInclCnt += ss->adjInclCnt;
  vgCnt += ss->vgCnt;
  vnwCnt += ss->vnwCnt;
  lidCnt += ss->lidCnt;
  vzCnt += ss->vzCnt;
  bwCnt += ss->bwCnt;
  twCnt += ss->twCnt;
  nounCnt += ss->nounCnt;
  nounInclCnt += ss->nounInclCnt;
  verbCnt += ss->verbCnt;
  verbInclCnt += ss->verbInclCnt;
  tswCnt += ss->tswCnt;
  specCnt += ss->specCnt;
  letCnt += ss->letCnt;
  betrCnt += ss->betrCnt;
  bijwCnt += ss->bijwCnt;
  complCnt += ss->complCnt;
  mvFinInbedCnt += ss->mvFinInbedCnt;
  infinComplCnt += ss->infinComplCnt;
  mvInbedCnt += ss->mvInbedCnt;
  losBetrCnt += ss->losBetrCnt;
  losBijwCnt += ss->losBijwCnt;

  allConnCnt += ss->allConnCnt;
  tempConnCnt += ss->tempConnCnt;
  opsomWgConnCnt += ss->opsomWgConnCnt;
  opsomZinConnCnt += ss->opsomZinConnCnt;
  contrastConnCnt += ss->contrastConnCnt;
  compConnCnt += ss->compConnCnt;
  causeConnCnt += ss->causeConnCnt;

  timeSitCnt += ss->timeSitCnt;
  spaceSitCnt += ss->spaceSitCnt;
  causeSitCnt += ss->causeSitCnt;
  emoSitCnt += ss->emoSitCnt;
  prepExprCnt += ss->prepExprCnt;
  propNegCnt += ss->propNegCnt;
  morphNegCnt += ss->morphNegCnt;
  multiNegCnt += ss->multiNegCnt;
  wordOverlapCnt += ss->wordOverlapCnt;
  lemmaOverlapCnt += ss->lemmaOverlapCnt;
  prevalenceP += ss->prevalenceP;
  prevalenceZ += ss->prevalenceZ;
  prevalenceContentP += ss->prevalenceContentP;
  prevalenceContentZ += ss->prevalenceContentZ;
  prevalenceCovered += ss->prevalenceCovered;
  prevalenceContentCovered += ss->prevalenceContentCovered;
  f50Cnt += ss->f50Cnt;
  f65Cnt += ss->f65Cnt;
  f77Cnt += ss->f77Cnt;
  f80Cnt += ss->f80Cnt;

  top1000Cnt += ss->top1000Cnt;
  top2000Cnt += ss->top2000Cnt;
  top3000Cnt += ss->top3000Cnt;
  top5000Cnt += ss->top5000Cnt;
  top10000Cnt += ss->top10000Cnt;
  top20000Cnt += ss->top20000Cnt;
  top1000ContentCnt += ss->top1000ContentCnt;
  top2000ContentCnt += ss->top2000ContentCnt;
  top3000ContentCnt += ss->top3000ContentCnt;
  top5000ContentCnt += ss->top5000ContentCnt;
  top10000ContentCnt += ss->top10000ContentCnt;
  top20000ContentCnt += ss->top20000ContentCnt;
  top1000ContentStrictCnt += ss->top1000ContentStrictCnt;
  top2000ContentStrictCnt += ss->top2000ContentStrictCnt;
  top3000ContentStrictCnt += ss->top3000ContentStrictCnt;
  top5000ContentStrictCnt += ss->top5000ContentStrictCnt;
  top10000ContentStrictCnt += ss->top10000ContentStrictCnt;
  top20000ContentStrictCnt += ss->top20000ContentStrictCnt;

  word_freq += ss->word_freq;
  word_freq_n += ss->word_freq_n;
  lemma_freq += ss->lemma_freq;
  lemma_freq_n += ss->lemma_freq_n;

  word_freq_strict += ss->word_freq_strict;
  word_freq_n_strict += ss->word_freq_n_strict;
  lemma_freq_strict += ss->lemma_freq_strict;
  lemma_freq_n_strict += ss->lemma_freq_n_strict;

  // Wopr forwards probabilities
  avg_prob10_fwd += ss->avg_prob10_fwd;
  avg_prob10_fwd_content += ss->avg_prob10_fwd_content;
  avg_prob10_fwd_ex_names += ss->avg_prob10_fwd_ex_names;
  avg_prob10_fwd_content_ex_names += ss->avg_prob10_fwd_content_ex_names;
  entropy_fwd += ss->entropy_fwd;
  entropy_fwd_norm += ss->entropy_fwd_norm;
  perplexity_fwd += ss->perplexity_fwd;
  perplexity_fwd_norm += ss->perplexity_fwd_norm;
  
  // Wopr backwards probabilities
  avg_prob10_bwd += ss->avg_prob10_bwd;
  avg_prob10_bwd_content += ss->avg_prob10_bwd_content;
  avg_prob10_bwd_ex_names += ss->avg_prob10_bwd_ex_names;
  avg_prob10_bwd_content_ex_names += ss->avg_prob10_bwd_content_ex_names;
  entropy_bwd += ss->entropy_bwd;
  entropy_bwd_norm += ss->entropy_bwd_norm;
  perplexity_bwd += ss->perplexity_bwd;
  perplexity_bwd_norm += ss->perplexity_bwd_norm;

  intensCnt += ss->intensCnt;
  intensBvnwCnt += ss->intensBvnwCnt;
  intensBvbwCnt += ss->intensBvbwCnt;
  intensBwCnt += ss->intensBwCnt;
  intensCombiCnt += ss->intensCombiCnt;
  intensNwCnt += ss->intensNwCnt;
  intensTussCnt += ss->intensTussCnt;
  intensWwCnt += ss->intensWwCnt;
  generalNounCnt += ss->generalNounCnt;
  generalNounSepCnt += ss->generalNounSepCnt;
  generalNounRelCnt += ss->generalNounRelCnt;
  generalNounActCnt += ss->generalNounActCnt;
  generalNounKnowCnt += ss->generalNounKnowCnt;
  generalNounDiscCnt += ss->generalNounDiscCnt;
  generalNounDeveCnt += ss->generalNounDeveCnt;
  generalVerbCnt += ss->generalVerbCnt;
  generalVerbSepCnt += ss->generalVerbSepCnt;
  generalVerbRelCnt += ss->generalVerbRelCnt;
  generalVerbActCnt += ss->generalVerbActCnt;
  generalVerbKnowCnt += ss->generalVerbKnowCnt;
  generalVerbDiscCnt += ss->generalVerbDiscCnt;
  generalVerbDeveCnt += ss->generalVerbDeveCnt;
  generalAdverbCnt += ss->generalAdverbCnt;
  specificAdverbCnt += ss->specificAdverbCnt;
  smainCnt += ss->smainCnt;
  ssubCnt += ss->ssubCnt;
  sv1Cnt += ss->sv1Cnt;
  clauseCnt += ss->clauseCnt;
  correctedClauseCnt += ss->correctedClauseCnt;
  smainCnjCnt += ss->smainCnjCnt;
  ssubCnjCnt += ss->ssubCnjCnt;
  sv1CnjCnt += ss->sv1CnjCnt;
  presentCnt += ss->presentCnt;
  pastCnt += ss->pastCnt;
  subjonctCnt += ss->subjonctCnt;
  pron1Cnt += ss->pron1Cnt;
  pron2Cnt += ss->pron2Cnt;
  pron3Cnt += ss->pron3Cnt;
  persRefCnt += ss->persRefCnt;
  pronRefCnt += ss->pronRefCnt;
  strictNounCnt += ss->strictNounCnt;
  broadNounCnt += ss->broadNounCnt;
  strictAdjCnt += ss->strictAdjCnt;
  broadAdjCnt += ss->broadAdjCnt;
  subjectiveAdjCnt += ss->subjectiveAdjCnt;
  abstractWwCnt += ss->abstractWwCnt;
  concreteWwCnt += ss->concreteWwCnt;
  undefinedWwCnt += ss->undefinedWwCnt;
  undefinedATPCnt += ss->undefinedATPCnt;
  stateCnt += ss->stateCnt;
  actionCnt += ss->actionCnt;
  processCnt += ss->processCnt;
  humanAdjCnt += ss->humanAdjCnt;
  emoAdjCnt += ss->emoAdjCnt;
  nonhumanAdjCnt += ss->nonhumanAdjCnt;
  shapeAdjCnt += ss->shapeAdjCnt;
  colorAdjCnt += ss->colorAdjCnt;
  matterAdjCnt += ss->matterAdjCnt;
  soundAdjCnt += ss->soundAdjCnt;
  nonhumanOtherAdjCnt += ss->nonhumanOtherAdjCnt;
  techAdjCnt += ss->techAdjCnt;
  timeAdjCnt += ss->timeAdjCnt;
  placeAdjCnt += ss->placeAdjCnt;
  specPosAdjCnt += ss->specPosAdjCnt;
  specNegAdjCnt += ss->specNegAdjCnt;
  posAdjCnt += ss->posAdjCnt;
  negAdjCnt += ss->negAdjCnt;
  evaluativeAdjCnt += ss->evaluativeAdjCnt;
  epiPosAdjCnt += ss->epiPosAdjCnt;
  epiNegAdjCnt += ss->epiNegAdjCnt;
  abstractAdjCnt += ss->abstractAdjCnt;
  undefinedNounCnt += ss->undefinedNounCnt;
  uncoveredNounCnt += ss->uncoveredNounCnt;
  undefinedAdjCnt += ss->undefinedAdjCnt;
  uncoveredAdjCnt += ss->uncoveredAdjCnt;
  uncoveredVerbCnt += ss->uncoveredVerbCnt;
  humanCnt += ss->humanCnt;
  nonHumanCnt += ss->nonHumanCnt;
  artefactCnt += ss->artefactCnt;
  concrotherCnt += ss->concrotherCnt;
  substanceConcCnt += ss->substanceConcCnt;
  foodcareCnt += ss->foodcareCnt;
  timeCnt += ss->timeCnt;
  placeCnt += ss->placeCnt;
  measureCnt += ss->measureCnt;
  dynamicConcCnt += ss->dynamicConcCnt;
  substanceAbstrCnt += ss->substanceAbstrCnt;
  dynamicAbstrCnt += ss->dynamicAbstrCnt;
  nonDynamicCnt += ss->nonDynamicCnt;
  institutCnt += ss->institutCnt;
  npCnt += ss->npCnt;
  indefNpCnt += ss->indefNpCnt;
  npSize += ss->npSize;
  vcModCnt += ss->vcModCnt;
  vcModSingleCnt += ss->vcModSingleCnt;
  adjNpModCnt += ss->adjNpModCnt;
  npModCnt += ss->npModCnt;
  smallCnjCnt += ss->smallCnjCnt;
  smallCnjExtraCnt += ss->smallCnjExtraCnt;
  if ( ss->dLevel >= 0 ){
    if ( dLevel < 0 )
      dLevel = ss->dLevel;
    else
      dLevel += ss->dLevel;
  }
  dLevel_gt4 += ss->dLevel_gt4;
  impCnt += ss->impCnt;
  questCnt += ss->questCnt;
  nerCnt += ss->nerCnt;
  compoundCnt += ss->compoundCnt;
  compound3Cnt += ss->compound3Cnt;
  charCntNoun += ss->charCntNoun;
  charCntNonComp += ss->charCntNonComp;
  charCntComp += ss->charCntComp;
  charCntHead += ss->charCntHead;
  charCntSat += ss->charCntSat;
  charCntNounCorr += ss->charCntNounCorr;
  charCntCorr += ss->charCntCorr;
  word_freq_log_noun += ss->word_freq_log_noun;
  word_freq_log_non_comp += ss->word_freq_log_non_comp;
  word_freq_log_comp += ss->word_freq_log_comp;
  word_freq_log_head += ss->word_freq_log_head;
  word_freq_log_sat += ss->word_freq_log_sat;
  word_freq_log_head_sat += ss->word_freq_log_head_sat;
  word_freq_log_noun_corr += ss->word_freq_log_noun_corr;
  word_freq_log_corr += ss->word_freq_log_corr;
  word_freq_log_corr_strict += ss->word_freq_log_corr_strict;
  word_freq_log_n_corr += ss->word_freq_log_n_corr;
  word_freq_log_n_corr_strict += ss->word_freq_log_n_corr_strict;
  top1000CntNoun += ss->top1000CntNoun;
  top1000CntNonComp += ss->top1000CntNonComp;
  top1000CntComp += ss->top1000CntComp;
  top1000CntHead += ss->top1000CntHead;
  top1000CntSat += ss->top1000CntSat;
  top1000CntNounCorr += ss->top1000CntNounCorr;
  top1000CntCorr += ss->top1000CntCorr;
  top5000CntNoun += ss->top5000CntNoun;
  top5000CntNonComp += ss->top5000CntNonComp;
  top5000CntComp += ss->top5000CntComp;
  top5000CntHead += ss->top5000CntHead;
  top5000CntSat += ss->top5000CntSat;
  top5000CntNounCorr += ss->top5000CntNounCorr;
  top5000CntCorr += ss->top5000CntCorr;
  top20000CntNoun += ss->top20000CntNoun;
  top20000CntNonComp += ss->top20000CntNonComp;
  top20000CntComp += ss->top20000CntComp;
  top20000CntHead += ss->top20000CntHead;
  top20000CntSat += ss->top20000CntSat;
  top20000CntNounCorr += ss->top20000CntNounCorr;
  top20000CntCorr += ss->top20000CntCorr;
  updateCounter(my_classification, ss->my_classification);
  sv.push_back( ss );
  aggregate( heads, ss->heads );
  aggregate( unique_names, ss->unique_names );
  aggregate( unique_contents, ss->unique_contents );
  aggregate( unique_contents_strict, ss->unique_contents_strict );
  aggregate( unique_words, ss->unique_words );
  aggregate( unique_lemmas, ss->unique_lemmas );
  aggregate( unique_tijd_sits, ss->unique_tijd_sits );
  aggregate( unique_ruimte_sits, ss->unique_ruimte_sits );
  aggregate( unique_cause_sits, ss->unique_cause_sits );
  aggregate( unique_emotion_sits, ss->unique_emotion_sits );
  aggregate( unique_all_conn, ss->unique_all_conn );
  aggregate( unique_temp_conn, ss->unique_temp_conn );
  aggregate( unique_reeks_wg_conn, ss->unique_reeks_wg_conn );
  aggregate( unique_reeks_zin_conn, ss->unique_reeks_zin_conn );
  aggregate( unique_contr_conn, ss->unique_contr_conn );
  aggregate( unique_comp_conn, ss->unique_comp_conn );
  aggregate( unique_cause_conn, ss->unique_cause_conn );
  aggregate( ners, ss->ners );
  aggregate( afks, ss->afks );
  aggregate( distances, ss->distances );
  al_gem = getMeanAL();
  al_max = getHighestAL();
}
