#ifndef _TICKET_USER_SYSTEM_H_
#define _TICKET_USER_SYSTEM_H_

#include "header.h"

namespace dark {

class user_system {
  private:
    using set_t = external_hash_set <account,8093>;

    set_t user_set; /* Set of user data. */
    account temp;   /* Temp account info. */


  public:

    /* Read in users. */
    user_system(std::string __path) : user_set(__path + "user")
    { for(auto &&iter : user_set) iter.login() = false; }

    ~user_system() = default;

    /* Return whether add_user succeed. */
    bool add_user(const char *__c,const char *__u,const char *__p,
                  const char *__n,const char *__m,const char *__g) {
        temp.login() = false;
        temp.count() = 0;

        if(user_set.empty()) { /* First insert. */
            temp.copy(__u,__p,__n,__m,10);
            user_set.insert(temp);
            return true;
        }

        auto *__a = user_set.find(string_hash(__c)); /* Account pointer. */
        auto  lvl = to_privilege(__g);   /* Real level. */

        /* No current user || Not logged in || Not enough level.  */
        if(!__a || __a->login() == false || __a->level() <= lvl) return false;

        /* Such user exists. */
        temp.user = __u;
        if(user_set.exist(temp)) return false;

        /* Be ready to insert into the map. */
        temp.pswd = __p;
        temp.name = __n;
        temp.mail = __m;
        temp.level() = lvl;
        user_set.insert(temp);
        return true;
    }


    /* Return whether login succeed. */
    bool login(const char *__u,const char *__p) {
        auto *__a = user_set.find(string_hash(__u)); /* Account pointer. */
        
        /* No such user || User has logged in || password is wrong   */
        if(!__a || __a->login() || strcmp(__a->pswd.base(),__p) != 0) return false;
        else return (__a->login() = true);
    }


    /* Return whether logout succeed. */
    bool logout(const char *__u) {
        auto __a = user_set.find(string_hash(__u));
        /* No such user || Not logged in. */
        if(!__a || !__a->login()) return false;
        return !(__a->login() = false);
    }


    /* Return pointer to user's profile. */
    account *query_profile(const char *__c,const char *__u) {
        auto *__a = user_set.find(string_hash(__c)); /* Account pointer of __c. */

        /* No current user || Not logged in || Not enough level.  */
        if(!__a || __a->login() == false) return nullptr;
    
        /* Identical __c and __u. */
        if(strcmp(__c,__u) == 0) return __a;

        auto *__t = user_set.find(string_hash(__u)); /* Account pointer of __u. */
        /* No target user || Not enough level */
        if(!__t || __a->level() <= __t->level()) return nullptr;
        else return __t;
    }

    /* Return pointer to user's profile. */
    account *modify_profile(const char *__c,const char *__u,const char *__p,
                            const char *__n,const char *__m,const char *__g) {
        auto *__a = user_set.find(string_hash(__c)); /* Account pointer of __c. */

        /* No current user || Not logged in || Not enough level.  */
        if(!__a || __a->login() == false) return nullptr;
    
        decltype(__a) __t; /* Lazy...... */
        if(strcmp(__c,__u) != 0) { /* Not identical __c and __u. */
            __t = user_set.find(string_hash(__u)); /* Account pointer of __u. */
            /* No target user || Not enough level */
            if(!__t || __a->level() <= __t->level()) return nullptr;
        } else __t = __a;

        privilege_t lvl;
        /* Not enough level. */
        if(__g && (__a->level() <= (lvl = to_privilege(__g)) ) ) return nullptr;

        if(__p) __t->pswd = __p;
        if(__n) __t->name = __n;
        if(__m) __t->mail = __m;
        if(__g) __t->level() = lvl;

        return __t;
    }

    /* Return pointer to count of __u's orders. */
    short *query_order(const char *__u) {
        auto *__a = user_set.find(string_hash(__u));
        return __a ? &__a->count() : nullptr;   
    }


};



}


#endif
