/*
 * input = two strings
 * goal = compare both strings to see if every word (separated by " ") in string A is also present in string B and vice versa, NOT case sensitive, extra whitespaces can be ignored
 *
 * Single struct method
 */ 

#ifndef __PROGTEST__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#endif /* __PROGTEST__ */

#define TRUE 1
#define FALSE 0
#define DELIM " "
#define INIT_SIZE_ARR 32
#define INIT_SIZE_STR 1

//dummy type for word_list
typedef void *list_t;

typedef struct word_list {
    int list_size;  //total size of list
    int list_used;  //used size
    char **list;    //string array
} word_list;

/**
 * @param char* str
 * @return char*
 *
 * create copy of input const string
 */
char *get_copy(const char *str);

/**
 * @param char* str
 *
 * correct format of input string - remove extra spaces and lowercase all characters
 */
void str_format(char *str);

/**
 * @return list_t
 *
 * create new list_t
 */
list_t init_list();

/**
 * @param list_t in
 *
 * free memory allocated by list_t in and its contents
 */
void free_list(list_t in);

/**
 * @param list_t in
 * @param int new_size
 *
 * change size of list_t in to new_size
 */
void list_resize(list_t in, int new_size);

/**
 * @param list_t in
 * @param char* str
 *
 * parse words from string (str) to list_t (in)
 * !will change contents of str, str can't be constant!
 */
void parse_words(list_t in, char *str);

/**
 *
 * @param list_t a
 * @param list_t b
 * @return int (TRUE|FALSE)
 *
 * compare lists, return false if mismatch, true otherwise
 */
int list_compare(list_t a, list_t b);

/**
 * @param void* a
 * @param void* b
 * @return int
 *
 * compare function for strings
 */
int compare(const void *a, const void *b);

/**
 * @param list_t in
 *
 * remove duplicate words from list_t
 */
void list_remove_duplicates(list_t in);

int sameWords(const char *a, const char *b) {
    //init lists, create copies of const inputs
    //a_list, b_list, a_copy, b_copy are dynamically allocated -> MUST BE FREED
    list_t a_list = init_list();
    list_t b_list = init_list();
    char *a_copy = get_copy(a);
    char *b_copy = get_copy(b);

    //correct format, parse words into lists
    str_format(a_copy);
    str_format(b_copy);
    parse_words(a_list, a_copy);
    parse_words(b_list, b_copy);

    //compare lists
    int cmp = list_compare(a_list, b_list);

    //free dynamic arrays
    free(a_copy);
    free(b_copy);
    free_list(a_list);
    free_list(b_list);
    return cmp;
}

char *get_copy(const char *str) {
    size_t size = strlen(str);
    char *copy = (char *) calloc(size, sizeof(char));
    for (size_t i = 0; i < size; i++) {
        copy[i] = str[i];
    }

    return copy;
}

void str_format(char *str) {
    int i, j;

    //remove extra whitespaces and lowercase all characters
    for (i = j = 0; str[i]; ++i) {
        if ((!isspace(str[i]) || (i > 0 && !isspace(str[i - 1]))) && (isalpha(str[i]) || isspace(str[i]))) {
            str[j++] = tolower(str[i]);
        }
    }

    str[j] = '\0';
}

list_t init_list() {
    word_list *wlist = (word_list *) malloc(sizeof(word_list));
    wlist->list_size = INIT_SIZE_ARR;
    wlist->list_used = 0;
    wlist->list = (char **) calloc(INIT_SIZE_ARR, sizeof(char *));

    for (int i = 0; i < INIT_SIZE_ARR; i++) {
        wlist->list[i] = (char *) calloc(INIT_SIZE_STR, sizeof(char));
    }

    return wlist;
}

void free_list(list_t in) {
    word_list *wlist = (word_list *) in;
    for (int i = 0; i < wlist->list_size; i++) {
        free(wlist->list[i]);
    }

    free(wlist->list);
    free(wlist);
}

void list_resize(list_t in, int new_size) {
    word_list *wlist = (word_list *) in;
    wlist->list_size = new_size;
    wlist->list = (char **) realloc(wlist->list, wlist->list_size * sizeof(char *));
    for (int i = wlist->list_used; i < wlist->list_size; i++) {
        wlist->list[i] = (char *) calloc(INIT_SIZE_STR, sizeof(char));
    }
}

void parse_words(list_t in, char *str) {
    word_list *wlist = (word_list *) in;
    char *parsed = strtok(str, DELIM); //get first word from str

    int index = 0;

    while (parsed != NULL) { //str is empty if parsed == NULL
        if (wlist->list_size == wlist->list_used) {
            list_resize(in, 2 * wlist->list_size);
        }

        wlist->list[index] = (char *) realloc(wlist->list[index],
                                              strlen(parsed) + 1); //change size of list[index] to size of parsed
        strcpy(wlist->list[index], parsed);

        index++;
        wlist->list_used++;
        parsed = strtok(NULL, DELIM); //get next word from original string
    }
}

int list_compare(list_t a, list_t b) {
    word_list *wlist_a = (word_list *) a;
    word_list *wlist_b = (word_list *) b;


    //sort both lists alphabetically
    qsort(wlist_a->list, wlist_a->list_used, sizeof wlist_a->list[0], compare);
    qsort(wlist_b->list, wlist_b->list_used, sizeof wlist_b->list[0], compare);

    //remove duplicates from lists
    list_remove_duplicates(a);
    list_remove_duplicates(b);

    //false is used sizes differ
    if (wlist_a->list_used != wlist_b->list_used) {
        return FALSE;
    }

    //iterate over list_a and compare contents to list_b, false if mismatch is found
    for (int i = 0; i < wlist_a->list_used; i++) {
        if (strcmp(wlist_a->list[i], wlist_b->list[i])) {
            return FALSE;
        }
    }

    //lists match
    return TRUE;
}

int compare(const void *a, const void *b) {
    char **ch_a = (char **) a;
    char **ch_b = (char **) b;

    return strcmp(*ch_a, *ch_b);
}

void list_remove_duplicates(list_t in) {
    word_list *wlist = (word_list *) in;
    int i, j;
    int n = wlist->list_used;


    //iterate over sorted list and remove duplicates
    for (i = j = 0; i < n; i++) {
        if (strcmp(wlist->list[i], wlist->list[i + 1])) {
            wlist->list[j] = (char *) realloc(wlist->list[j], strlen(wlist->list[i]) + 1);
            strcpy(wlist->list[j], wlist->list[i]);
            j++;
        }
    }

    free(wlist->list[j]);
    wlist->list[j] = (char *) calloc(strlen(wlist->list[n]) + 1, sizeof(char));
    strcpy(wlist->list[j++], wlist->list[n]);

    //resize to free unused memory
    list_resize(in, j);
    wlist->list_used = j;
}

#ifndef __PROGTEST__

int main(int argc, char *argv[]) {
    assert(sameWords(
            "              HAPBqYQDmPqJulDVBYOyl ZvCruEhdxBqAzcXmcbp  VpxZEVzzaAHbPW   kpMkTCpuCiOMarCVLs    TwudVyKwFtAhibSaMl     SaLqpXWjMGTzOWZtN      nesGJGhCLc       xUkxNb        TjvjeaiThBLnLnSEOcx DqoAikfpfpOogestMz  dVZHiP   SqJAUksuDQXuLKVFFjHXoY    jrx     ROcFzqkOWbPuwfUPion      wR       anKKMzZZrLSaSSwtPAsYWwm        fQbmrxpeiskgWNCMM pbl  ghDjMwzkOxni   ",
            "             ZvCruEhdxBqAzcXmcbp wR  kpMkTCpuCiOMarCVLs   DqoAikfpfpOogestMz    pbl     fQbmrxpeiskgWNCMM      anKKMzZZrLSaSSwtPAsYWwm       jrx        SqJAUksuDQXuLKVFFjHXoY VpxZEVzzaAHbPW  dVZHiP   TjvjeaiThBLnLnSEOcx    xUkxNb     ROcFzqkOWbPuwfUPion      HAPBqYQDmPqJulDVBYOyl       nesGJGhCLc        SaLqpXWjMGTzOWZtN TwudVyKwFtAhibSaMl  ghDjMwzkOxni   ") ==
           1);
    assert(sameWords(
            "qie%l3emp@gj%m_c1(]e_#.!vme__%.h&&ia#!mh )axc1xt!u_1uzugtav_fs1ko;u;y4_fe11la;vb4 ko(!u(1ciw[_]ciu(&1#d]yt%cb4s;ve_m4g%jbt la[m]oq4&ttyz)p(_!3^1a[1[s%!13(fpbh^q[[h !jg_h#h^1!!!'l4.!'y1&[]!ozs^y'4_!hz_f!;w a_ev;1l^i^_j![_!z!(%hxtt3_as%((l4a@(%p.[ coxq.z)(s(weqtjc('_h#xx(&ll'&3a(ass;_ihw o!#31!;v&hsbe!jc!!yzx4@yax4%o.qj)kuy1(yt si&trb(%!f(!vao&d(g33!iosac!(k!zw@ig^y^( mc4t@sgl#@az@cp.'1j.!hxyt!1rltpgls&(wg!_ 1e&kcc!@1!]x^rz#@(me(q!s(ryf%_h#4f@!&(_' qe&]3u]z'u^cqaf1x!b;!q3haq.(]@_tqsv4'e(b sdcj!;;e!tvzhb_^[jyul1)_.f(3tu(y&g(vit1! !g!1bx34v(!d!iwgfs;owwwk!kpzf!#fw%3'lw(j _t!!pp_kxbwj!it&1'qz((]b[z(4fp3)i3!i(v!3 ]jh_^v_]m1)h(;@zb&h[x1)ft(caj!r!_;g_bzjr tcwv!@!d%eq1]x!y4j#1yk!l[s%&)wsa_.ab%!t1 sl(_k;'4!%bv_l4(3i4%h_k;;'3@!.(3qx1qvj![ m!hh%o!i_duqby'q!(1vsy!c'a^au4xd]#m)g;4a g!x_set4g@t[j@;w4xq%w(v_()zwr!x(1yd!h!ai !q[pxo_e.'[s[z#y#si'wy]b!!i(v.r!.!')3@sk bsdq4h1he&yf)!qvxgz;1b1'ke1c]l#i(&z14ixa &g1vxc(p!;1[my4_g.1m@po&csz_@(x(@r!lba%! rbuh4e!x[yi1smchbqiwom4^hi!t)sxibs(fz;pu %q_[_1!hcrp@!_b&1_'i!&')(q%k_tm@1s!#k(rp vu]#@rcrrrqwu_d.4_c_[_]^li1@e[^c_4lhp(e( !p&lpw&ds](rtu1pryq1_qq3rh@#%.lfca].uz!^ g3!y;jwp_l_!^tb_1wy!!'da1)wzbb_'3p.vhrw! f)r&1sfpt!]e3j!#e&act_&gdtjexxhvbgy_egrl wc'f_et%t!('#c(zjk1v!i__#iqzt!u!;g!_hp&g g&y!wf]qg(_vvsb##&.1.cb@]ph1;)!1^xopyi!k y4;!c_a!ra_jtppz.!p&t_@fci(_!_!;3lt!te%! iw_h1t!jwvf!qxtql(hj#z@jah_^@il[@^'uwi!4 e!#h#ys@fu__!!e.o1sk1[)ied1#t!1t;;y)i';g [i_p^d.'@ak;wg(m_%!lalz(;!t1so#c;ql!3z&! &!ib'_j%.q)gt'&tex')4_ifg)xds#!l1j!#]#1w _le@.!#zu1;tqm_'3'[_)_(%1ds!ut![ej;x)[__ !g@!qstzx!xcl^j!)#a!)md#s3w!mihx[fhg)m#. 1_(@li.spx(1z#(@^)a_11jxfyl'me;#1wtoklgq .rlm(sfo(elf@#mvm3'.ky_fju!q#hp)1x&(!&d; wo3yd()jm'd(dc&atxq!!h!rdqha!#!'gua.oc@^ l!%o[@%1jsw!ij@]pej]!qw&%dw41_v_(!!.@v.# _v_qfr_];i4.#xhz'ky_x#l4!wyftgdy3&'aa%@r [gm#43vhk1u%_yyw3b@j)%j;&tbe%]ecp;cs_#u; )3savfz;vkp_dh;%4la@3el(kher)]h_gy&3f!'! !]jf(a^'1(l[_m_lvb_ardl.;%cr4_m_!)^y_(#o k%u.%(#1uplg_z#xugsvi!eb1'w&eukehj!!!!4! mx&e(z%yg_m;iu]l@t_f_vh_ph^qt[qyk1.!sw%_ ym1u.g_]wr4!d4ja!f_c.u@_l)_zo!jd'a_g3vhc tbyv^)ub1ky!k_sg_zi&;kal1_'r1z_azugtw!x; de)zfk!;b_;i((wtozx(jy!dhx%sq1osq!(kt^bw @1%.']_;!!a4_amwg;#]q](_]gzds.avthx%z!l1 qa(4vb].!o(rj(dda!c1yt_']4(^yk!x(@mt^^s_ eurda(ri@bb^'mp[))!^s!u%u!q_(1z1#!g(s1(_ qthpios^!p!1e@#@.&m_(!wgs)(h('t_q'_[!(bb @ks;1mvs_a1uy_1.('1t)34a[fs4yc^)!'g!dk%! j__!!klkohrkex&madwbi!ykxh!m!bd!]h1(hs1b &@ubh!kqrty(fx(lv'j;zud@(xxotx_s_sao3c!l dsp)frzgb1]vc(qtreu!'11o])r&u!m1xscta.zz ]4sbm!!t1!^dbch13.d!1^d([w_p]f.i@mz(^vgr &_cu4^f^.kjyr;^!m%%oiy1s;ib_(yu)#jbv(s4d .31q4ekd#p!xwml4pu].314%_3_3)j.ae&!!^#!' yj(hmzy]@uojt&u(!a!_!t!_x%v#h!s(r)lv'j]_ lx1ave1py^_;')f1vic(v[!uq)^@qqh3a)zh!cxd pqp)os]_kmd4;q#)di).q[!.!(i1%!gp_bp(kp#) 4(d(f.jxc13!tr!dl!;ii3!4_jvt([g3_(sj&%3r ca!r!zeo^a@l;g@l4;p1voze.vqq;a)^crsk;t1c ^rq]x#&hf(_!%!;i!3pg!cm#^.sd'lby_h_t4v1] ^1!p;(#1!va_klc_(1v1rx^)]@!%qk(1)!.!br%w _doslbj'h_x^r!..!p'h4k_1_4l[uzv3l'.vr(vf k!d!.dv4bpyk[_t_3pa1@rtls4et%4z!gt@zz)f! _b(cap!vlh'_[p1&]4(3xev!j[h1crk!1a%.1f(^ 1;([u_cp1!4qqf(%fxx!h)1d!)^(^ue!e31[u#!& x_'r!(_!x_k(#vc1]or.jsp(]u;ta!a3)qdg!1t# ]i_h1;achev31vvwh!!_3v__!4o1g!%%rp_1(h!( @[%ze%!s#(@e;wkd[m;tg[^sa#dxx!v_]r)r&a@4 _c1w!o.1j1g^r!_rh@(c1p!1w'!]&.f!1vcyq)ls m[z!lux%4_)ihq!o4cj_(g.rh[@(.(y!__!z!wi] s_1m@e!41%_)ug[_(gzv@1#(1';p#smxg[[%_f.l ]mro('1o_!;gkyt.rrmh1i&e.wtw(a@gm@u!]_yb g1[oa['#bc^qdi!%#l%_(3[_!k_i1^f!i%%(_ysd 11g[;x_@[ck_u1_hh1.ejlu!mp_%owzy@eau1e%q w3tipt'g1_;uk!#j_@ex%[wuksydbwcf)4zg&;t] [^y)^1iiac1g!k(tq3fwod'ho!kgf.(4&@xd_#st dhy[)i4b_1!;aru)%q!b_ie1.[kxr(@)t1b&km]_ ;rb'y&yjdwpf4v11]t^ro11kaahp!i3_fj1as(13 'a%@((&a#'3;(&!1(1qap.4^!iu%3_f4];wyk!!h gwi1;u)3y;r&o3ha!1u!ts!a3j!1cel@_tt&]qly !%v@&loop_q@!s4_pw!kzk3blfy#zdg!hw!!!(f# j_!!_!r]'4q'ty@4&%wcihyo%o);!1@].hdlg._1 1d#ul]beycdp[_biwx!o!jaqc^zd^#taw_ogpyz[ _ypppu)acxb;m@[a%z(d__'1w44&chj1af_x_@.1 ;#]tfr_c^.a^@.]oe!uhitbesc&!.#zfk1r4!^.x %;^y_;ba_!;_%%lq!ah^!#x!frgfwp!..f3)1.;h !;!ejci!agh@pzzw!wtu1_l;!dt()r)!wbe)!!uq v%!!vicp)_^]w1_f]@!_g1ahzowdrj3&'1'#a;m( @a)e(pz_![()zjoe#&zsz!!dfu@_b%o_]v;xdy1@ #@e(jm(rt3v#pxbmz^s]ksv^rc)pkq_(__!g1_%i sk'(l1&u1f(!zspoo4v^f)s%!_!#!_%y;;!vyq_. w%m!c]e1(_x!(1&1&41_]1%!!&dy4ai(&!w_.cjj 3!.k.qt[^%qg@'lx(4)il;u)vr@dvc&dg^ox[z.( ^1p(1'(ot1!rl1s^iz41s!!pzyicxje.!%_!q#@' ][po#u!rk_bydrolx_(al;_qx4'yt_s1!1es)!um 4[m1;j#(&x!q]%p#^wfb%dsi4k@u3![4@agr1!1k &p]i]az_3xf;qpww_hu__43p'^v1kz![sk;^(scc mi!(g4x&3.'w_ih.x'!_3k;z_k__jrho3!ri(rg_ 'dvx.y![4'syvm1vtd_zu![pf#(k!u(!jh[l(wu! e^(id^!j;e%%wwc;hhm#v]!c;(%wfc%__%lhg1w[ qoc^ww!cap1@o!z_%;hi'_fafv![v11cdv1!hp4d sy11!1p^mi1#(%gz1@xj([bcze(@!!(@%!f!^srs [ks_k^;hler&%'_!%bm(zau_i@4')g(uri!;p)1! y[@td!(.!ui4q%z&!@ue[m!)!]ed@d!qh^e4k4@s epcg@jk!k.e4r_hpzj%bw.1&qr(t!e1y;zg!mx1e _a'f(%][wzc^vfgw'm]gp!h[1!_[v;#cb1!lmuz& !f]r_1'bg_eq1).#jf__mj('u1u]^ljwtut!@a!w i([_]!!3w!f(s_h_!y!3@r#r1ctu'1(v(ha3[o4b b!x_vc&^q(_;hl!!#@gjuybf4smxl(#.!((!mp%v !)orw!^43t%&^(rg[(k's_pyfxy1(rkr)mq!_l._ ug3ymje&@&gv!!!ke%]1qk!v_!uqm!;m_(q&ohpx ^kgk.ccxm!@&k11bz[i3'!rf;33iw(o3i@zp11eh erx@]^%'^f((!m(^kv[_!^utv#c]!v_a@1!gma1' d.k)(p!!)e%[_qyo!m^i!3yqu_3yjd4sfmgam3_c _ivj!#q_w1!ouqtvmw#f!'q%p;)duh41!tif&!w; '!_(!ox@)ory11'1y!hahtpwf1!'umuqkyok.ytm qvflfhdojd_.@)(&1('_!!zor(alfu[z)_%[dr[) s4jyx[z(](kdpy(!uja[v(v]ees_hcw1_!1gduj] gw@og_f[(.!cf14(1p;_i(ciz3[v_tofhq(ewil3 japrla.^t_#__)!ak&x]p(k!dty%(zc;a1^l@_te t!kp3!1!;i@s!3c[1_')w!fzt'x!3s'4v^a!zee( b&(!(sj11t^z1r!_fh1!)gybsp&!1.e^!#jm!pj^ !#(!&4@tu_ka'_4d''rq!of_(ej;_uea(&tqgr(q 1x14!e_^q[y[1m_#l1(r';hmv%#1e%^).sg#!@_q ]!p&d']m'o_j1;cb1(z!gk&'_z]@(i_l3h);1#g. ^1gjdh!t4)(%1ej4!l#giuik!3]'e_sdm)#hgviz kjvz#1].m3pkr(ha(weuy^w3_]u.dxs_k@e(g@(4 3dphr[a)1rpd!lr;^1%ije!(i!kl_pvh!1m!w^_& v1[z.l;uxd1_jf!u'y1ck'_;sv^ck![coyr!xb([ ^mk]%^!bp_'el.ul^kqw4k((''s'tv&!b(!.r#ja wigslfj;f1v_abh&1_b#'1zl&zqbv_t(v.qj4](; !4ztgr)_!!!c@ajl;c1zdazb_a)'1#h)''g!_'%( j]_r(!3w!mk%_&e;il!ea!!m!p'.e!)!;o(u1_t3 )[]vmt!copmqv_&t4g##)(v]m[#g%!&ltv1vk^cc #41i3usqsh_!3i@a!fv'v(o.^e_i@mx!l11_#a1l ;^wlpesk(f4@_!(m3a4pv]d^jochw41_k^_hb[rj %#f1yt3ewd(__a&[(@(@at3!fdt];1lv3_[rw(q( j]dmfyyjk1^__4_zu;iq]')!buc3.[!!3r)iw!t. '43!mdyp_!cfodfqg'_i1&@ow1_urdx.'_;%^gth vgwl!ua)!js(!^1@z4ob3fs'pr;rmk.!]!!sx(o( m^ujejo1'l_)1a)((w)@m)cv4_f1(z([!!@_g^o_ y)!1!tw&h]h_']yu!'w^4d(%iip[!cpk^%!m'_4a m!c.'vfl((j43.iobv'lwv!z!ka4tmq!3%if(m&i o1ot;%[](txev_l)z1btq#okh@_3(&('4a(is3fq 1@^@_m_.!_ezl3'#1#e1_j(._(^[cr!4e!!]t&1p @w^zb)_zdo!q)[i3qxxf&zq4w!!sv_s[j!_y[zko j[(o;d#!s3e1h3!1(c%!qc_(i_3^d!3fxl[l^(1] %]q[_oh!!c_aurcx%q;;q%1j!uyfwxfys'1z]hyh #bexhlf[fx3xzl4(b!#;(41]s_h3!!h;u_w(h'!s !e[1gvgg_w3g3s#!%d'ha)%3e!;'tvc#rr[]mj#p (44kkl!gkm'm]b4](1[haxpj;_o@a!1s_vfds!s! #h_r3vx4&1f_!tubrz!m'.xf)]%g;'i31!!hbs') e].bv&!x!1mysibrr1cbo!utmriybh'z(1x;zf^d ok!_xkapb]43pdy@(_h(^;3(s!4i(ge.dwf(lab_ )k)h!_](rwwmzs!(#tgv]#]((c(s(]pkg((mphcy r#!c!c!o]i!.4#&ady]mm(_^@_b%_qq!^v#b_a^g xt)e!!^^v.iqey1dw;s];fc]!fy__f4(l%@__]!t &]uv43hx!!1#3mcazq]bu!qp4[!woo_.zsbz(_1] xo!ki#if_.4s3k)m!!j!%xpi(1@'4^@!.@3'ql_! yd!4o&ju3zvi&f__yr&f.%ij1x3s1q_!^1;^zt&! wzvxmdq;_!rpa;1ughcrl!glf&.1vqj(_^jq_f&3 u]4[o^eu^^!vh^!b!_o%;gdix!!!hwtd!1_rzx!) ..y@wbhpf![t1y(d!#w(dq1moemehj'a_'aprf;g e;b!z!lyr[p1y^^!re1@ht(ytiu%[zptqquy[lj! r_[!z!!]3he^c_orjm_!sd;_ed]jtdgq!t)ldee] kggd_w'_w1bmcg!^qv!zsw!_b13a)!kk__zm^g@v z1d!)_!hjrqmyp_hj#p#y1x%_^!)wy(ajx!_zwo( ty(b([wgq1v'g'v&dk!!%;x1#upgeqa&yrxddu_3 &l4qaca[r@e1@(qsw!k.[;iae]mzzy![vgvx;)ec 3u!tx(z_(^!#g(_&g#tc&!)t;__]].!ic(1_kbl! 3&;ofmu%f%z3w(tyej1(i.my!o&@(h4;.xasg_jv r]kifut;.4rm_z.pg!)i1]_wrmaho1p4m#a]&lge r!bw_ix[_#](r;wx!fd3%k_f1z)!h_(#ys![;b]# coem4&);[['vb_&gy!jv)z)e'_w_!@e_hdhyoi!) y@c!w1._xd!vtpsw#]1&ctc_!k_y.ai((c(@da(1 e4_@dtt]er1;@ccfdvzz'33ad'v1p(m)emmft[]u hh!uf!!so;_grmveh[@14#31]c((f[3!(%up^[rg dxs!t1w1!(1.f_!dz!l!_za^fdq(3ks!y_!&;&4c l.bky!!wmc[x_^;e(3^#_4@c(f_&)mtqp@_c1q!! @up^[%;[)!y)!m#_elj3(!r@va&^(q.fcmar(j4u &u;o!!b;_!ghd4%c_1#es(xqtbm.oc#zi]g!&u!g mupud';i%u[o_lckm;hr]vm%a![_d!ua_(z[d%rz sw!(c!)k3^bzih_gl!k)!x%ds]]s4m@a!!!z1cz# d!ejw@3v[3x!z!oohs4_w1o!m@bvd'[#a'hm&!;! rzq'(.vl!)!ua1m.#wbluuwsw^moc_q.p]m_v)(! fheukqjsm&1bqot'1f!fac[!%(e;r_!3bxp1rs]u 3!j@e@ixf[]ia1ar%dsu3xy&(_&a1j]gb(tw_!.e ^3;lkm#!1pi[x#al._x_](u!la1d3i]!s1hv&j[) 3sfvb!m!(mpzg[rfmp(4]fg1'h[!&'^'h([!h!lv .r%!z__e&qctck^s]y#tq!k]%!.rvi!y_.crl(lz  rwkplri!_)u%i!@dfj'h!ftdc1%t(f_j!kk^%h_e bt_w_db&jofc11zfmvay!((fwo)qvlp!l4#xe3(u g__j&3ge_.ouu];o.!t;_da1qzv';!(re11#d@ya %@l1_pv1z4(vl.1]f(f(egic3s1e&_31au%3ea_^ !4ayteu&!amm!]1'wa_qrb_3x(.(!d@!@@y3!r1[ [!d(p1b#pvs]bqo!s%;b]_[4d%1mbkkf_4ftv!4. et(xoclw!.]lk4!#zjq(_g^%!'1%w)ym#w!.[vku w))vkz[)dor_ta(43[x&!!!l_h!_;%p!)!!o4lqo q)%lh1qhurdujtu4s!l;1_(!o!x_a!w!).sxqdbt !![l.^d^[gedukdd@lty'u@_se;k(!3y;c)sh@#( cf_[]c1_141@1;mu#@hkmocs!g#_#m&@gi4xj((d etikyyy^q4r&vv%xjjmi4j'r_ws!a#zshq;!;jq( 1^uo&&c^k!]&!!_ecb;y!(__;!!u(@];_jioyddo u#3!@'y)3x!!h3q^@ly_fk_scc1gxs!s1u!oj.1q zs!.gw13%xkt]t!'j%qm1[vww!@'!^[xpex[r^3z [a3uz)!kutsuy[zoy'pwvp!!b[@_pt!ic1^@y.j1 !at!kck^(__#o&@#3gt)o13b(.irp&c1_q!.dk_! %bicmsieab1.!s1pz@mo[_grk]i)wd(xjs1jc^1j pff_q;[os.!f!1ppp_(fp!s&qiji!gqvbtm@3wzt 1j(_xcp)1_'!)im!.co@e&#c&lpqulh@ix#1vpw[ '']x%shq)wo^y]vzw_rule;ui(irxgwqx[!.jbjj h_rxhrjs1z4br[_w;3)^]tf(_o[#z(r!gkhh((hw 1_@)hz1js1xge4fl!ik;;!f_[[!cdeaboj__s@e@ q1&gvz4uuzv_%%i1!vw!d&_akrfr)y)z.(1_&]!y 4ibi!_]44uek!j!fv3#kgza;z;4%y_^%xfis#_!y [;m..![^p'!acb3)1_o'^!;pzk#]44v.3&j;[._s t.ay3j%odg%vowcg_k3g3o_qi!!xemk_so^'ob@; awxp^yj!r_dj!zie_%;(@izs1[q&%@&(&m;#@^k1 _!o4bi!!_gr&(jh%_q;q!(!eo_s._dwdf1j1wxrc l(m_!i1%g1q]3bpau[cs]q!_!_dg31epdzc!ar_! %w]1oc.!_@fjio(!v%@'x1._da@&gdg_)s_jvcb_ jdif#_!s;!1lh1(t(1.^(igc'!v]4q]z^_3^y1'm '_!d!l!__y!];elsm!p1_;11.1!x.jy(4'((#w4r h1hm1e#.3my^v!etm#o#)u'kl!c!!h_#f!@;zujc y3@t1qsvs!]g!ydcg&j(c&t!!db_fuhgs)!bk!cb 3oqzfup3!utj4v!4kqjy(1u(rrebw^_vg3_!!e^_ c!h&p^&!])v_c(vmc_e^vmj.dg@g1jk1]!(a[hab !lz1e1scsiy@__pz&;!zjmrsi_(puxtrd^j(uz!1 m!!!qy3!o!(ehbuvr[)hq)j#4s^)imzh1yr;ybl! !r1rxkc_u%z4_#lel[hw@mv^4.3zwirg[v11i)q4 '&)@'_'yyu)@1c1o'e!e'1g(.3!;meort!__h!ag k^[e#bu^4!pwok]m#!3kqd4e%wsv[[!!l_oby[o! mmff4@idevad^;#!ji]p._3#m^!rl!k@1fsr]p%s k_zk3oel_ss!!!rc]4yj^lpmzw[s'st'fc]r;glz ejkctc(k1(aw]alukyvf!3%fyx1zl]jbwi.b&.;) _![(!ga]k_d'(#ip)j331!(4;okk!&]1vh.v!yqw pc'!^f[^crz'y#d)%q#w'ob;f1)ls_fq(#bkti!q _r!_vv_x^4]pu^cfrm1ursow#3t[;_3imzu4jt]3 bq#zy^#dq_@h1^1!cf4p#]x3_e(#zr__!xjf3.)u wkmtd&al^d]'@!;&(c_#q(!i11_([pjoh!q_('oe qsqt(gm_a@jb(]#%bqv];qw(!o_rx@zv.k_q!u_w )4o)!tb^pa%m1_p;!!yhmf1_.!o!cy)v@rluol1a a(4aq!u_eu1d@;3[ao!w_v(jpzzgwvv!qa&&a!p# h[obb]4ib)_)(y;#amy%@bc;x!!!k_)&(z^a;w'! (;y%!u)bz_1vm3%u(!z(g]y%;;%b@ds)_l_c[z_q 3&!p;d(!ap#m!vxqf)().h(!xtol)!@l(!'q_@jq .r_hl^p!fsd!h(1!wqh((bhck3#i'&tq%1_;!bm( #^hadq]1_t%v!4a!'y!!&(osu!e['x)fc)w3q#__ #@y^[l#k&rfh_[@.__toz_!v##!hz4!bi1_!s!a1 uf!!__tfcwavb^cj%vtfe#1stj!qgxgmp!%!yc_g sh!al^eq)]_v4i&3vo!vwkb&x3y[1yry_ftzuv!( ri^4%cc'g&]i_tp!_d__^fwy1@mjdojfw31l@(r; vb_']s'w[lgyw)(xs.ciuq.j^gpi#p#d_l_3.d)c #ev%eut4t)')pxohz^44e^h_gtu!h!smw;@ktq(1 rqq.o;ap%&4%er(]pejg#_wh[gge@k%[1li.()y! 3c)&;p;k!wm.!x(!l13icmo#m'v'(e_]'.a]_;(r ('kiropqc3ho_3;z@c11w@w&.')g!ujx1ip1co.e x#t)jd_.1b_&p@e]!fz;e'_&'jxss(b[1m(k_ggv ve_#rbtm__(uctcf4_ztszc]j1.uvjx11v.4[y'e [duqro.j.yfk.]q!1)!1lzy(m^khciw@!.]jfdac .3h^tgya)^&f_;_.hw;1ses!y4k_z^x!'ymh)ate 1]y(3&w&!4_c_'^_dp3_pzqke^!bm_uhc1ae!_;! w_!lgx;!&ez_(oshlitysm1q#yavr!fs(w!%w_#l ')h[!g3gvel[]b1_q@_l##_)lbvct[v!(kx4b.1k !f__^]#3s&l^;xl@svl!(_;._ek#uch!o_[ffjz( !_]k&pz.]3_g!!r#c@a_[xd!zkbjrva!_&(e1]k^ ![[cf'_s!se(g%1jd!.c^@.%k[.jva@!_c_jx!'f e!e!_(eevxf)]s!1dsmp&!(j(vqc[ct4tba1wb!x j3)cw!kwfuo#fp1^_aejd)!kg!!f)]y].mrjt!vk (whd.)a%g1f''!_]'cw(cgmqm1%'^y.@ml1cb_[e a%u^]v^__ad;q_['mi3gfvaxrip@#&qw_4gp.ysv f![iak3af1tl&@.f;;@ic(!^_j_1k;tgj;g[pd!_ ayx!1ycs;!!&'fuajt#;z(p111![d_r!1!bs4;_! 3d^;_afgwuq(zz)gyubzcu.km_(p(ua1qwigp_!b ja3.@[hdy!!13p#c]z)'_mu14;z@@e'd()&3[x#g jir%sf!3m)vh!#[lr1i!(4@s(]1ccbia_^(fp'@1 ie_gghy;;(mv1@h!o[b&r(rf^![a)qh@w[b(!t4& [spv]4#i]c[!y__]!vfgoo@1mskf_sqpy_y4kj;o rb3t^!4a3!#zg).tglb'kh)y[@!y;bdmh!&ajudk ]yhp^4&[tr#mxv!gcy3qtg!pbs!w)!(qjz1!pw)s %q%&e[.!h&[&c!1f'_gj4!&zd#w^'.mt&qq&!d!e tgp&wksw!'!_(q1_pxefa!_tdq^d((4.j3o_i](! !@z!s&l!(;[ygu1!'#j1__p]ph(!xa)#.yf)iwx3 (q[rot!%gye[!ggka#fiokma1emu!'3!ka#!aye' cdd'p!a_gl[r'a.t__am^'!p'qv_'vd1@&dm'3z% q1%x.qucf;usb'%^1!1ll_!h()&crt#_@r]e!!bk @#^xbxu([)t.m'q;p!()[[x^tic![_s!bf)(_my# (_#sp'!v&trewd11c(off(.rlv(4]!#mzy'%j1[^ __.zv4)fh;((&!1[!!mwtp&1^qg][s![k4g3%!_j glb1i!hiiw%;k(x!%b!qlj]'_u'z)ukalm;io'y( @.1ll!g^3hk!tj!wavecai@itqtli3o_.1wq#@e^ .do__.](ll#abk4kqxiyqs@4_j14_._u1l]y%;b( [!!1r!o_(_!r!1c#t%w(w'4ai(!'g1(;.]([;b!d s[bj_@wrt_)csub!!pqu@g)&.zf!1]imqt'qtoce _a.'y'pq#kq1;x_xql!4z!rdhjcm!oqbwmtqyslu hl)od_i.zje;!b@3_cdcyw!u.c#ycf(xt]grzd;m iwx^yk_e1(_!d(a3w%fy_d()rt_;1]]i!!fd().1 ]_;xsth(di(c.b.vmc3__gyb!hd1!_j3iwb![iow ^)!!^e(!;1'ay_]lv!!3vqceieam_h]zc_..13e[ me[su_]umi]]j).!btie1yybq;sm!h1gk_)!d.td (m&[%(!.]_y]_qb[.([_ef]xi_sp1@d'x[]k]x!) oh%gx@1c@w%p_x)ml&pcmt)u(m@v([1!vzd@1^_! (fd;l1zryjqexxp@(][u(;gvkhr!_z4w(!!;m!%% l!1fotsv[uv^bwjld_1'!mr^!lr&(o).ke(e)3zb )w@p&%'kr.__arqr[zml)3@mdhbm^ft&^x_)q_z4 b[q_hg%_ye_c[a1kg!&_z!!#'!e3;olj(!su1u4) #b!q(apx@#_a.r4)_rhe'gwcex(z1zyimh&!oop! __)yab!.(_o3sb3__%_3m3iea[_ju!abs;!b&hid .^_!.%!!!_ifp1mfxpeh11(el')h_#;j%b%ac'hz &'gkzp!cy_j@z(!!!##[;pxp4_;]1e!vlt1))hbf gchuf!ctt!l(uv.up'!p([ibxd1&^ag#w)d#.q%3 l!bm'red1g3h!o(_qw&1r&_%q;(jmf_![!tiukm_ u#;aazth4.l_r_^xygyi!3@3!]j__fj#q_!d!_b% qaycb3x]gzem[1ab.___f)i([_k4x!q4)rm'#uv_ s!ei[!!pvyy[;ofxjc#!q#]x1!3x.^4)jw11ira. v_wfjjp'be^ivc^3ry4c(4'w_]m!3f(;!tmh![zw uxva@pk%^3c(t!zesqge((uk^gz_t(4d(r_j.urv jdq_1v_4)!%_fc!s)&x!_#&1!iujzxh!a;_g(!ht au'!p@)h'v_(_bqv&lb_fj#f3_e^kz]1!gk^;]a1 hjo(b(g!!wsxxf_k1@!ao[#i&c!xx_ets.f^hpd4 g(!)bp)yj_o]!h);ds_z!%g_do.l'mp!&huk'w(4 eo_3h^rt1!_el)pu]^##;!h3(@[_mxzz4pc'3_&i _[v(^je;ih_g.s&(wa1!pi.s41m!^_'apcx)%t(! em.i_!z!g&b@l@moe!'x;!u%%1dmwd.cw[q@^!wm !ckw_w3;sdv_%jc#(s!mv%^lpzmdpao_3xot^!]& )j!(%'!a(e%;lmr)mfljdtt;dsfhm3hm_^.1x&4s %1j!_1_)[(zukqj_1w!%z!^e!x@p;4!l!a3e''oa bou'ib%q(_f1wllcsam1!_^(@.v#z!y_]i_m!1c# ^^13^!_s]jj!vh.l@(wi33@4]&;se.3#4!ep.@q. d&#ep]u!^tj4!^&brfr4g[!tasfc^iw_[(_sp!h% iwa^qog%mo.kq.a_(sr1[1]_(;dmsi!(;g;_j(g' lhgm_zfx)!tq!!;[h(h!g4#!%r'mr;;@z4orszm( lr1wx1#fvwmudo3jz1pd;tjb1!&sc(](3ueb;!c& 1hiuwm!_^&_k)!.mpbu##gr;!bk1!l'jyk[&)ilf p!.&yfp!(!g(]!]3z^1]ed.qc_ysm.gozvz#s4jm ",
            "qie%l3emp@gj%m_c1(]e_#.!vme__%.h&&ia#!mh )axc1xt!u_1uzugtav_fs1ko;u;y4_fe11la;vb4 ko(!u(1ciw[_]ciu(&1#d]yt%cb4s;ve_m4g%jbt la[m]oq4&ttyz)p(_!3^1a[1[s%!13(fpbh^q[[h !jg_h#h^1!!!'l4.!'y1&[]!ozs^y'4_!hz_f!;w a_ev;1l^i^_j![_!z!(%hxtt3_as%((l4a@(%p.[ coxq.z)(s(weqtjc('_h#xx(&ll'&3a(ass;_ihw o!#31!;v&hsbe!jc!!yzx4@yax4%o.qj)kuy1(yt si&trb(%!f(!vao&d(g33!iosac!(k!zw@ig^y^( mc4t@sgl#@az@cp.'1j.!hxyt!1rltpgls&(wg!_ 1e&kcc!@1!]x^rz#@(me(q!s(ryf%_h#4f@!&(_' qe&]3u]z'u^cqaf1x!b;!q3haq.(]@_tqsv4'e(b sdcj!;;e!tvzhb_^[jyul1)_.f(3tu(y&g(vit1! !g!1bx34v(!d!iwgfs;owwwk!kpzf!#fw%3'lw(j _t!!pp_kxbwj!it&1'qz((]b[z(4fp3)i3!i(v!3 ]jh_^v_]m1)h(;@zb&h[x1)ft(caj!r!_;g_bzjr tcwv!@!d%eq1]x!y4j#1yk!l[s%&)wsa_.ab%!t1 sl(_k;'4!%bv_l4(3i4%h_k;;'3@!.(3qx1qvj![ m!hh%o!i_duqby'q!(1vsy!c'a^au4xd]#m)g;4a g!x_set4g@t[j@;w4xq%w(v_()zwr!x(1yd!h!ai !q[pxo_e.'[s[z#y#si'wy]b!!i(v.r!.!')3@sk bsdq4h1he&yf)!qvxgz;1b1'ke1c]l#i(&z14ixa &g1vxc(p!;1[my4_g.1m@po&csz_@(x(@r!lba%! rbuh4e!x[yi1smchbqiwom4^hi!t)sxibs(fz;pu %q_[_1!hcrp@!_b&1_'i!&')(q%k_tm@1s!#k(rp vu]#@rcrrrqwu_d.4_c_[_]^li1@e[^c_4lhp(e( !p&lpw&ds](rtu1pryq1_qq3rh@#%.lfca].uz!^ g3!y;jwp_l_!^tb_1wy!!'da1)wzbb_'3p.vhrw! f)r&1sfpt!]e3j!#e&act_&gdtjexxhvbgy_egrl wc'f_et%t!('#c(zjk1v!i__#iqzt!u!;g!_hp&g g&y!wf]qg(_vvsb##&.1.cb@]ph1;)!1^xopyi!k y4;!c_a!ra_jtppz.!p&t_@fci(_!_!;3lt!te%! iw_h1t!jwvf!qxtql(hj#z@jah_^@il[@^'uwi!4 e!#h#ys@fu__!!e.o1sk1[)ied1#t!1t;;y)i';g [i_p^d.'@ak;wg(m_%!lalz(;!t1so#c;ql!3z&! &!ib'_j%.q)gt'&tex')4_ifg)xds#!l1j!#]#1w _le@.!#zu1;tqm_'3'[_)_(%1ds!ut![ej;x)[__ !g@!qstzx!xcl^j!)#a!)md#s3w!mihx[fhg)m#. 1_(@li.spx(1z#(@^)a_11jxfyl'me;#1wtoklgq .rlm(sfo(elf@#mvm3'.ky_fju!q#hp)1x&(!&d; wo3yd()jm'd(dc&atxq!!h!rdqha!#!'gua.oc@^ l!%o[@%1jsw!ij@]pej]!qw&%dw41_v_(!!.@v.# _v_qfr_];i4.#xhz'ky_x#l4!wyftgdy3&'aa%@r [gm#43vhk1u%_yyw3b@j)%j;&tbe%]ecp;cs_#u; )3savfz;vkp_dh;%4la@3el(kher)]h_gy&3f!'! !]jf(a^'1(l[_m_lvb_ardl.;%cr4_m_!)^y_(#o k%u.%(#1uplg_z#xugsvi!eb1'w&eukehj!!!!4! mx&e(z%yg_m;iu]l@t_f_vh_ph^qt[qyk1.!sw%_ ym1u.g_]wr4!d4ja!f_c.u@_l)_zo!jd'a_g3vhc tbyv^)ub1ky!k_sg_zi&;kal1_'r1z_azugtw!x; de)zfk!;b_;i((wtozx(jy!dhx%sq1osq!(kt^bw @1%.']_;!!a4_amwg;#]q](_]gzds.avthx%z!l1 qa(4vb].!o(rj(dda!c1yt_']4(^yk!x(@mt^^s_ eurda(ri@bb^'mp[))!^s!u%u!q_(1z1#!g(s1(_ qthpios^!p!1e@#@.&m_(!wgs)(h('t_q'_[!(bb @ks;1mvs_a1uy_1.('1t)34a[fs4yc^)!'g!dk%! j__!!klkohrkex&madwbi!ykxh!m!bd!]h1(hs1b &@ubh!kqrty(fx(lv'j;zud@(xxotx_s_sao3c!l dsp)frzgb1]vc(qtreu!'11o])r&u!m1xscta.zz ]4sbm!!t1!^dbch13.d!1^d([w_p]f.i@mz(^vgr &_cu4^f^.kjyr;^!m%%oiy1s;ib_(yu)#jbv(s4d .31q4ekd#p!xwml4pu].314%_3_3)j.ae&!!^#!' yj(hmzy]@uojt&u(!a!_!t!_x%v#h!s(r)lv'j]_ lx1ave1py^_;')f1vic(v[!uq)^@qqh3a)zh!cxd pqp)os]_kmd4;q#)di).q[!.!(i1%!gp_bp(kp#) 4(d(f.jxc13!tr!dl!;ii3!4_jvt([g3_(sj&%3r ca!r!zeo^a@l;g@l4;p1voze.vqq;a)^crsk;t1c ^rq]x#&hf(_!%!;i!3pg!cm#^.sd'lby_h_t4v1] ^1!p;(#1!va_klc_(1v1rx^)]@!%qk(1)!.!br%w _doslbj'h_x^r!..!p'h4k_1_4l[uzv3l'.vr(vf k!d!.dv4bpyk[_t_3pa1@rtls4et%4z!gt@zz)f! _b(cap!vlh'_[p1&]4(3xev!j[h1crk!1a%.1f(^ 1;([u_cp1!4qqf(%fxx!h)1d!)^(^ue!e31[u#!& x_'r!(_!x_k(#vc1]or.jsp(]u;ta!a3)qdg!1t# ]i_h1;achev31vvwh!!_3v__!4o1g!%%rp_1(h!( @[%ze%!s#(@e;wkd[m;tg[^sa#dxx!v_]r)r&a@4 _c1w!o.1j1g^r!_rh@(c1p!1w'!]&.f!1vcyq)ls m[z!lux%4_)ihq!o4cj_(g.rh[@(.(y!__!z!wi] s_1m@e!41%_)ug[_(gzv@1#(1';p#smxg[[%_f.l ]mro('1o_!;gkyt.rrmh1i&e.wtw(a@gm@u!]_yb g1[oa['#bc^qdi!%#l%_(3[_!k_i1^f!i%%(_ysd 11g[;x_@[ck_u1_hh1.ejlu!mp_%owzy@eau1e%q w3tipt'g1_;uk!#j_@ex%[wuksydbwcf)4zg&;t] [^y)^1iiac1g!k(tq3fwod'ho!kgf.(4&@xd_#st dhy[)i4b_1!;aru)%q!b_ie1.[kxr(@)t1b&km]_ ;rb'y&yjdwpf4v11]t^ro11kaahp!i3_fj1as(13 'a%@((&a#'3;(&!1(1qap.4^!iu%3_f4];wyk!!h gwi1;u)3y;r&o3ha!1u!ts!a3j!1cel@_tt&]qly !%v@&loop_q@!s4_pw!kzk3blfy#zdg!hw!!!(f# j_!!_!r]'4q'ty@4&%wcihyo%o);!1@].hdlg._1 1d#ul]beycdp[_biwx!o!jaqc^zd^#taw_ogpyz[ _ypppu)acxb;m@[a%z(d__'1w44&chj1af_x_@.1 ;#]tfr_c^.a^@.]oe!uhitbesc&!.#zfk1r4!^.x %;^y_;ba_!;_%%lq!ah^!#x!frgfwp!..f3)1.;h !;!ejci!agh@pzzw!wtu1_l;!dt()r)!wbe)!!uq v%!!vicp)_^]w1_f]@!_g1ahzowdrj3&'1'#a;m( @a)e(pz_![()zjoe#&zsz!!dfu@_b%o_]v;xdy1@ #@e(jm(rt3v#pxbmz^s]ksv^rc)pkq_(__!g1_%i sk'(l1&u1f(!zspoo4v^f)s%!_!#!_%y;;!vyq_. w%m!c]e1(_x!(1&1&41_]1%!!&dy4ai(&!w_.cjj 3!.k.qt[^%qg@'lx(4)il;u)vr@dvc&dg^ox[z.( ^1p(1'(ot1!rl1s^iz41s!!pzyicxje.!%_!q#@' ][po#u!rk_bydrolx_(al;_qx4'yt_s1!1es)!um 4[m1;j#(&x!q]%p#^wfb%dsi4k@u3![4@agr1!1k &p]i]az_3xf;qpww_hu__43p'^v1kz![sk;^(scc mi!(g4x&3.'w_ih.x'!_3k;z_k__jrho3!ri(rg_ 'dvx.y![4'syvm1vtd_zu![pf#(k!u(!jh[l(wu! e^(id^!j;e%%wwc;hhm#v]!c;(%wfc%__%lhg1w[ qoc^ww!cap1@o!z_%;hi'_fafv![v11cdv1!hp4d sy11!1p^mi1#(%gz1@xj([bcze(@!!(@%!f!^srs [ks_k^;hler&%'_!%bm(zau_i@4')g(uri!;p)1! y[@td!(.!ui4q%z&!@ue[m!)!]ed@d!qh^e4k4@s epcg@jk!k.e4r_hpzj%bw.1&qr(t!e1y;zg!mx1e _a'f(%][wzc^vfgw'm]gp!h[1!_[v;#cb1!lmuz& !f]r_1'bg_eq1).#jf__mj('u1u]^ljwtut!@a!w i([_]!!3w!f(s_h_!y!3@r#r1ctu'1(v(ha3[o4b b!x_vc&^q(_;hl!!#@gjuybf4smxl(#.!((!mp%v !)orw!^43t%&^(rg[(k's_pyfxy1(rkr)mq!_l._ ug3ymje&@&gv!!!ke%]1qk!v_!uqm!;m_(q&ohpx ^kgk.ccxm!@&k11bz[i3'!rf;33iw(o3i@zp11eh erx@]^%'^f((!m(^kv[_!^utv#c]!v_a@1!gma1' d.k)(p!!)e%[_qyo!m^i!3yqu_3yjd4sfmgam3_c _ivj!#q_w1!ouqtvmw#f!'q%p;)duh41!tif&!w; '!_(!ox@)ory11'1y!hahtpwf1!'umuqkyok.ytm qvflfhdojd_.@)(&1('_!!zor(alfu[z)_%[dr[) s4jyx[z(](kdpy(!uja[v(v]ees_hcw1_!1gduj] gw@og_f[(.!cf14(1p;_i(ciz3[v_tofhq(ewil3 japrla.^t_#__)!ak&x]p(k!dty%(zc;a1^l@_te t!kp3!1!;i@s!3c[1_')w!fzt'x!3s'4v^a!zee( b&(!(sj11t^z1r!_fh1!)gybsp&!1.e^!#jm!pj^ !#(!&4@tu_ka'_4d''rq!of_(ej;_uea(&tqgr(q 1x14!e_^q[y[1m_#l1(r';hmv%#1e%^).sg#!@_q ]!p&d']m'o_j1;cb1(z!gk&'_z]@(i_l3h);1#g. ^1gjdh!t4)(%1ej4!l#giuik!3]'e_sdm)#hgviz kjvz#1].m3pkr(ha(weuy^w3_]u.dxs_k@e(g@(4 3dphr[a)1rpd!lr;^1%ije!(i!kl_pvh!1m!w^_& v1[z.l;uxd1_jf!u'y1ck'_;sv^ck![coyr!xb([ ^mk]%^!bp_'el.ul^kqw4k((''s'tv&!b(!.r#ja wigslfj;f1v_abh&1_b#'1zl&zqbv_t(v.qj4](; !4ztgr)_!!!c@ajl;c1zdazb_a)'1#h)''g!_'%( j]_r(!3w!mk%_&e;il!ea!!m!p'.e!)!;o(u1_t3 )[]vmt!copmqv_&t4g##)(v]m[#g%!&ltv1vk^cc #41i3usqsh_!3i@a!fv'v(o.^e_i@mx!l11_#a1l ;^wlpesk(f4@_!(m3a4pv]d^jochw41_k^_hb[rj %#f1yt3ewd(__a&[(@(@at3!fdt];1lv3_[rw(q( j]dmfyyjk1^__4_zu;iq]')!buc3.[!!3r)iw!t. '43!mdyp_!cfodfqg'_i1&@ow1_urdx.'_;%^gth vgwl!ua)!js(!^1@z4ob3fs'pr;rmk.!]!!sx(o( m^ujejo1'l_)1a)((w)@m)cv4_f1(z([!!@_g^o_ y)!1!tw&h]h_']yu!'w^4d(%iip[!cpk^%!m'_4a m!c.'vfl((j43.iobv'lwv!z!ka4tmq!3%if(m&i o1ot;%[](txev_l)z1btq#okh@_3(&('4a(is3fq 1@^@_m_.!_ezl3'#1#e1_j(._(^[cr!4e!!]t&1p @w^zb)_zdo!q)[i3qxxf&zq4w!!sv_s[j!_y[zko j[(o;d#!s3e1h3!1(c%!qc_(i_3^d!3fxl[l^(1] %]q[_oh!!c_aurcx%q;;q%1j!uyfwxfys'1z]hyh #bexhlf[fx3xzl4(b!#;(41]s_h3!!h;u_w(h'!s !e[1gvgg_w3g3s#!%d'ha)%3e!;'tvc#rr[]mj#p (44kkl!gkm'm]b4](1[haxpj;_o@a!1s_vfds!s! #h_r3vx4&1f_!tubrz!m'.xf)]%g;'i31!!hbs') e].bv&!x!1mysibrr1cbo!utmriybh'z(1x;zf^d ok!_xkapb]43pdy@(_h(^;3(s!4i(ge.dwf(lab_ )k)h!_](rwwmzs!(#tgv]#]((c(s(]pkg((mphcy r#!c!c!o]i!.4#&ady]mm(_^@_b%_qq!^v#b_a^g xt)e!!^^v.iqey1dw;s];fc]!fy__f4(l%@__]!t &]uv43hx!!1#3mcazq]bu!qp4[!woo_.zsbz(_1] xo!ki#if_.4s3k)m!!j!%xpi(1@'4^@!.@3'ql_! yd!4o&ju3zvi&f__yr&f.%ij1x3s1q_!^1;^zt&! wzvxmdq;_!rpa;1ughcrl!glf&.1vqj(_^jq_f&3 u]4[o^eu^^!vh^!b!_o%;gdix!!!hwtd!1_rzx!) ..y@wbhpf![t1y(d!#w(dq1moemehj'a_'aprf;g e;b!z!lyr[p1y^^!re1@ht(ytiu%[zptqquy[lj! r_[!z!!]3he^c_orjm_!sd;_ed]jtdgq!t)ldee] kggd_w'_w1bmcg!^qv!zsw!_b13a)!kk__zm^g@v z1d!)_!hjrqmyp_hj#p#y1x%_^!)wy(ajx!_zwo( ty(b([wgq1v'g'v&dk!!%;x1#upgeqa&yrxddu_3 &l4qaca[r@e1@(qsw!k.[;iae]mzzy![vgvx;)ec 3u!tx(z_(^!#g(_&g#tc&!)t;__]].!ic(1_kbl! 3&;ofmu%f%z3w(tyej1(i.my!o&@(h4;.xasg_jv r]kifut;.4rm_z.pg!)i1]_wrmaho1p4m#a]&lge r!bw_ix[_#](r;wx!fd3%k_f1z)!h_(#ys![;b]# coem4&);[['vb_&gy!jv)z)e'_w_!@e_hdhyoi!) y@c!w1._xd!vtpsw#]1&ctc_!k_y.ai((c(@da(1 e4_@dtt]er1;@ccfdvzz'33ad'v1p(m)emmft[]u hh!uf!!so;_grmveh[@14#31]c((f[3!(%up^[rg dxs!t1w1!(1.f_!dz!l!_za^fdq(3ks!y_!&;&4c l.bky!!wmc[x_^;e(3^#_4@c(f_&)mtqp@_c1q!! @up^[%;[)!y)!m#_elj3(!r@va&^(q.fcmar(j4u &u;o!!b;_!ghd4%c_1#es(xqtbm.oc#zi]g!&u!g mupud';i%u[o_lckm;hr]vm%a![_d!ua_(z[d%rz sw!(c!)k3^bzih_gl!k)!x%ds]]s4m@a!!!z1cz# d!ejw@3v[3x!z!oohs4_w1o!m@bvd'[#a'hm&!;! rzq'(.vl!)!ua1m.#wbluuwsw^moc_q.p]m_v)(! fheukqjsm&1bqot'1f!fac[!%(e;r_!3bxp1rs]u 3!j@e@ixf[]ia1ar%dsu3xy&(_&a1j]gb(tw_!.e ^3;lkm#!1pi[x#al._x_](u!la1d3i]!s1hv&j[) 3sfvb!m!(mpzg[rfmp(4]fg1'h[!&'^'h([!h!lv .r%!z__e&qctck^s]y#tq!k]%!.rvi!y_.crl(lz  rwkplri!_)u%i!@dfj'h!ftdc1%t(f_j!kk^%h_e bt_w_db&jofc11zfmvay!((fwo)qvlp!l4#xe3(u g__j&3ge_.ouu];o.!t;_da1qzv';!(re11#d@ya %@l1_pv1z4(vl.1]f(f(egic3s1e&_31au%3ea_^ !4ayteu&!amm!]1'wa_qrb_3x(.(!d@!@@y3!r1[ [!d(p1b#pvs]bqo!s%;b]_[4d%1mbkkf_4ftv!4. et(xoclw!.]lk4!#zjq(_g^%!'1%w)ym#w!.[vku w))vkz[)dor_ta(43[x&!!!l_h!_;%p!)!!o4lqo q)%lh1qhurdujtu4s!l;1_(!o!x_a!w!).sxqdbt !![l.^d^[gedukdd@lty'u@_se;k(!3y;c)sh@#( cf_[]c1_141@1;mu#@hkmocs!g#_#m&@gi4xj((d etikyyy^q4r&vv%xjjmi4j'r_ws!a#zshq;!;jq( 1^uo&&c^k!]&!!_ecb;y!(__;!!u(@];_jioyddo u#3!@'y)3x!!h3q^@ly_fk_scc1gxs!s1u!oj.1q zs!.gw13%xkt]t!'j%qm1[vww!@'!^[xpex[r^3z [a3uz)!kutsuy[zoy'pwvp!!b[@_pt!ic1^@y.j1 !at!kck^(__#o&@#3gt)o13b(.irp&c1_q!.dk_! %bicmsieab1.!s1pz@mo[_grk]i)wd(xjs1jc^1j pff_q;[os.!f!1ppp_(fp!s&qiji!gqvbtm@3wzt 1j(_xcp)1_'!)im!.co@e&#c&lpqulh@ix#1vpw[ '']x%shq)wo^y]vzw_rule;ui(irxgwqx[!.jbjj h_rxhrjs1z4br[_w;3)^]tf(_o[#z(r!gkhh((hw 1_@)hz1js1xge4fl!ik;;!f_[[!cdeaboj__s@e@ q1&gvz4uuzv_%%i1!vw!d&_akrfr)y)z.(1_&]!y 4ibi!_]44uek!j!fv3#kgza;z;4%y_^%xfis#_!y [;m..![^p'!acb3)1_o'^!;pzk#]44v.3&j;[._s t.ay3j%odg%vowcg_k3g3o_qi!!xemk_so^'ob@; awxp^yj!r_dj!zie_%;(@izs1[q&%@&(&m;#@^k1 _!o4bi!!_gr&(jh%_q;q!(!eo_s._dwdf1j1wxrc l(m_!i1%g1q]3bpau[cs]q!_!_dg31epdzc!ar_! %w]1oc.!_@fjio(!v%@'x1._da@&gdg_)s_jvcb_ jdif#_!s;!1lh1(t(1.^(igc'!v]4q]z^_3^y1'm '_!d!l!__y!];elsm!p1_;11.1!x.jy(4'((#w4r h1hm1e#.3my^v!etm#o#)u'kl!c!!h_#f!@;zujc y3@t1qsvs!]g!ydcg&j(c&t!!db_fuhgs)!bk!cb 3oqzfup3!utj4v!4kqjy(1u(rrebw^_vg3_!!e^_ c!h&p^&!])v_c(vmc_e^vmj.dg@g1jk1]!(a[hab !lz1e1scsiy@__pz&;!zjmrsi_(puxtrd^j(uz!1 m!!!qy3!o!(ehbuvr[)hq)j#4s^)imzh1yr;ybl! !r1rxkc_u%z4_#lel[hw@mv^4.3zwirg[v11i)q4 '&)@'_'yyu)@1c1o'e!e'1g(.3!;meort!__h!ag k^[e#bu^4!pwok]m#!3kqd4e%wsv[[!!l_oby[o! mmff4@idevad^;#!ji]p._3#m^!rl!k@1fsr]p%s k_zk3oel_ss!!!rc]4yj^lpmzw[s'st'fc]r;glz ejkctc(k1(aw]alukyvf!3%fyx1zl]jbwi.b&.;) _![(!ga]k_d'(#ip)j331!(4;okk!&]1vh.v!yqw pc'!^f[^crz'y#d)%q#w'ob;f1)ls_fq(#bkti!q _r!_vv_x^4]pu^cfrm1ursow#3t[;_3imzu4jt]3 bq#zy^#dq_@h1^1!cf4p#]x3_e(#zr__!xjf3.)u wkmtd&al^d]'@!;&(c_#q(!i11_([pjoh!q_('oe qsqt(gm_a@jb(]#%bqv];qw(!o_rx@zv.k_q!u_w )4o)!tb^pa%m1_p;!!yhmf1_.!o!cy)v@rluol1a a(4aq!u_eu1d@;3[ao!w_v(jpzzgwvv!qa&&a!p# h[obb]4ib)_)(y;#amy%@bc;x!!!k_)&(z^a;w'! (;y%!u)bz_1vm3%u(!z(g]y%;;%b@ds)_l_c[z_q 3&!p;d(!ap#m!vxqf)().h(!xtol)!@l(!'q_@jq .r_hl^p!fsd!h(1!wqh((bhck3#i'&tq%1_;!bm( #^hadq]1_t%v!4a!'y!!&(osu!e['x)fc)w3q#__ #@y^[l#k&rfh_[@.__toz_!v##!hz4!bi1_!s!a1 uf!!__tfcwavb^cj%vtfe#1stj!qgxgmp!%!yc_g sh!al^eq)]_v4i&3vo!vwkb&x3y[1yry_ftzuv!( ri^4%cc'g&]i_tp!_d__^fwy1@mjdojfw31l@(r; vb_']s'w[lgyw)(xs.ciuq.j^gpi#p#d_l_3.d)c #ev%eut4t)')pxohz^44e^h_gtu!h!smw;@ktq(1 rqq.o;ap%&4%er(]pejg#_wh[gge@k%[1li.()y! 3c)&;p;k!wm.!x(!l13icmo#m'v'(e_]'.a]_;(r ('kiropqc3ho_3;z@c11w@w&.')g!ujx1ip1co.e x#t)jd_.1b_&p@e]!fz;e'_&'jxss(b[1m(k_ggv ve_#rbtm__(uctcf4_ztszc]j1.uvjx11v.4[y'e [duqro.j.yfk.]q!1)!1lzy(m^khciw@!.]jfdac .3h^tgya)^&f_;_.hw;1ses!y4k_z^x!'ymh)ate 1]y(3&w&!4_c_'^_dp3_pzqke^!bm_uhc1ae!_;! w_!lgx;!&ez_(oshlitysm1q#yavr!fs(w!%w_#l ')h[!g3gvel[]b1_q@_l##_)lbvct[v!(kx4b.1k !f__^]#3s&l^;xl@svl!(_;._ek#uch!o_[ffjz( !_]k&pz.]3_g!!r#c@a_[xd!zkbjrva!_&(e1]k^ ![[cf'_s!se(g%1jd!.c^@.%k[.jva@!_c_jx!'f e!e!_(eevxf)]s!1dsmp&!(j(vqc[ct4tba1wb!x j3)cw!kwfuo#fp1^_aejd)!kg!!f)]y].mrjt!vk (whd.)a%g1f''!_]'cw(cgmqm1%'^y.@ml1cb_[e a%u^]v^__ad;q_['mi3gfvaxrip@#&qw_4gp.ysv f![iak3af1tl&@.f;;@ic(!^_j_1k;tgj;g[pd!_ ayx!1ycs;!!&'fuajt#;z(p111![d_r!1!bs4;_! 3d^;_afgwuq(zz)gyubzcu.km_(p(ua1qwigp_!b ja3.@[hdy!!13p#c]z)'_mu14;z@@e'd()&3[x#g jir%sf!3m)vh!#[lr1i!(4@s(]1ccbia_^(fp'@1 ie_gghy;;(mv1@h!o[b&r(rf^![a)qh@w[b(!t4& [spv]4#i]c[!y__]!vfgoo@1mskf_sqpy_y4kj;o rb3t^!4a3!#zg).tglb'kh)y[@!y;bdmh!&ajudk ]yhp^4&[tr#mxv!gcy3qtg!pbs!w)!(qjz1!pw)s %q%&e[.!h&[&c!1f'_gj4!&zd#w^'.mt&qq&!d!e tgp&wksw!'!_(q1_pxefa!_tdq^d((4.j3o_i](! !@z!s&l!(;[ygu1!'#j1__p]ph(!xa)#.yf)iwx3 (q[rot!%gye[!ggka#fiokma1emu!'3!ka#!aye' cdd'p!a_gl[r'a.t__am^'!p'qv_'vd1@&dm'3z% q1%x.qucf;usb'%^1!1ll_!h()&crt#_@r]e!!bk @#^xbxu([)t.m'q;p!()[[x^tic![_s!bf)(_my# (_#sp'!v&trewd11c(off(.rlv(4]!#mzy'%j1[^ __.zv4)fh;((&!1[!!mwtp&1^qg][s![k4g3%!_j glb1i!hiiw%;k(x!%b!qlj]'_u'z)ukalm;io'y( @.1ll!g^3hk!tj!wavecai@itqtli3o_.1wq#@e^ .do__.](ll#abk4kqxiyqs@4_j14_._u1l]y%;b( [!!1r!o_(_!r!1c#t%w(w'4ai(!'g1(;.]([;b!d s[bj_@wrt_)csub!!pqu@g)&.zf!1]imqt'qtoce _a.'y'pq#kq1;x_xql!4z!rdhjcm!oqbwmtqyslu hl)od_i.zje;!b@3_cdcyw!u.c#ycf(xt]grzd;m iwx^yk_e1(_!d(a3w%fy_d()rt_;1]]i!!fd().1 ]_;xsth(di(c.b.vmc3__gyb!hd1!_j3iwb![iow ^)!!^e(!;1'ay_]lv!!3vqceieam_h]zc_..13e[ me[su_]umi]]j).!btie1yybq;sm!h1gk_)!d.td (m&[%(!.]_y]_qb[.([_ef]xi_sp1@d'x[]k]x!) oh%gx@1c@w%p_x)ml&pcmt)u(m@v([1!vzd@1^_! (fd;l1zryjqexxp@(][u(;gvkhr!_z4w(!!;m!%% l!1fotsv[uv^bwjld_1'!mr^!lr&(o).ke(e)3zb )w@p&%'kr.__arqr[zml)3@mdhbm^ft&^x_)q_z4 b[q_hg%_ye_c[a1kg!&_z!!#'!e3;olj(!su1u4) #b!q(apx@#_a.r4)_rhe'gwcex(z1zyimh&!oop! __)yab!.(_o3sb3__%_3m3iea[_ju!abs;!b&hid .^_!.%!!!_ifp1mfxpeh11(el')h_#;j%b%ac'hz &'gkzp!cy_j@z(!!!##[;pxp4_;]1e!vlt1))hbf gchuf!ctt!l(uv.up'!p([ibxd1&^ag#w)d#.q%3 l!bm'red1g3h!o(_qw&1r&_%q;(jmf_![!tiukm_ u#;aazth4.l_r_^xygyi!3@3!]j__fj#q_!d!_b% qaycb3x]gzem[1ab.___f)i([_k4x!q4)rm'#uv_ s!ei[!!pvyy[;ofxjc#!q#]x1!3x.^4)jw11ira. v_wfjjp'be^ivc^3ry4c(4'w_]m!3f(;!tmh![zw uxva@pk%^3c(t!zesqge((uk^gz_t(4d(r_j.urv jdq_1v_4)!%_fc!s)&x!_#&1!iujzxh!a;_g(!ht au'!p@)h'v_(_bqv&lb_fj#f3_e^kz]1!gk^;]a1 hjo(b(g!!wsxxf_k1@!ao[#i&c!xx_ets.f^hpd4 g(!)bp)yj_o]!h);ds_z!%g_do.l'mp!&huk'w(4 eo_3h^rt1!_el)pu]^##;!h3(@[_mxzz4pc'3_&i _[v(^je;ih_g.s&(wa1!pi.s41m!^_'apcx)%t(! em.i_!z!g&b@l@moe!'x;!u%%1dmwd.cw[q@^!wm !ckw_w3;sdv_%jc#(s!mv%^lpzmdpao_3xot^!]& )j!(%'!a(e%;lmr)mfljdtt;dsfhm3hm_^.1x&4s %1j!_1_)[(zukqj_1w!%z!^e!x@p;4!l!a3e''oa bou'ib%q(_f1wllcsam1!_^(@.v#z!y_]i_m!1c# ^^13^!_s]jj!vh.l@(wi33@4]&;se.3#4!ep.@q. d&#ep]u!^tj4!^&brfr4g[!tasfc^iw_[(_sp!h% iwa^qog%mo.kq.a_(sr1[1]_(;dmsi!(;g;_j(g' lhgm_zfx)!tq!!;[h(h!g4#!%r'mr;;@z4orszm( lr1wx1#fvwmudo3jz1pd;tjb1!&sc(](3ueb;!c& 1hiuwm!_^&_k)!.mpbu##gr;!bk1!l'jyk[&)ilf p!.&yfp!(!g(]!]3z^1]ed.qc_ysm.gozvz#s4jm ") ==
           1);
    assert(sameWords("He said he would do it.", "IT said: 'He would do it.'") == 1);
    assert(sameWords("one two three", "one two five") == 0);
    return 0;
}

#endif /* __PROGTEST__ */
