/* 
 * File:   Help.h
 * Author: Bob
 *
 * Created on July 6, 2020, 6:48 PM
 */

#ifndef HELP_H
#define	HELP_H

class Help 
{
public:
    Help(const char *primary, const char *aux1, const char *aux2, const char *helpText) :
        _primary(primary), _aux1(aux1), _aux2(aux2), _helpText(helpText)
        {}
        
    const char *Primary() const {return _primary;}
    const char *Aux1() const {return _aux1;}
    const char *Aux2() const {return _aux2;}
    const char *Text() const {return _helpText;}

private:
    Help(const Help& orig);
    
    const char *_primary;
    const char *_aux1;
    const char *_aux2;
    const char *_helpText;
};

#endif	/* HELP_H */

