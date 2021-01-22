/* 
 * File:   UtilityPane.h
 * Author: Bob
 *
 * Created on September 23, 2020, 1:29 PM
 */

#ifndef UTILITYPANE_H
#define	UTILITYPANE_H

#include "Pane.h"

class UtilityPane : public Pane
{
public:
    UtilityPane();
    virtual ~UtilityPane();
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    UtilityPane(const UtilityPane& orig);
};

#endif	/* UTILITYPANE_H */

