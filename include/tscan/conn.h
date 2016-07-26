#ifndef CONN_H
#define CONN_H

namespace Conn {

    enum Type {
        NOCONN, TEMPOREEL, OPSOMMEND_WG, OPSOMMEND_ZIN, CONTRASTIEF, COMPARATIEF, CAUSAAL
    };
    std::string toString(Type);
    std::ostream& operator<<( std::ostream&, const Type&);
}

#endif /* CONN_H */
