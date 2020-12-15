/*
 *
 *  Nolan Corcoran
 *  ndc466
 *
 */

import java.net.*;
import java.io.*;
import java.util.*;
import java.awt.Color;
import java.io.Serializable;

public class Message implements Serializable {   
    private static final long serialVersionUID = 1L;
    public Integer srcId, destId, core, level, cost;
    public Color color;
    public String name;
    public TreeMap<Integer, Integer> edges;
    public String info;

    public Message(Integer srcId, Integer destId) {
        this.srcId = srcId;
        this.destId = destId;
    }

    public String toString() {
        return "" + srcId + " --> " + destId;
    }

}

class Registration extends Message {
    public Registration(Integer destId, String name) {
        super(null, destId);
        this.info = "(Registration - " + name + ")";
        this.name = name;
        this.color = Color.blue;
    }
}

class Confirmation extends Message {
    public Confirmation(Integer destId, TreeMap<Integer, Integer> edges) {
        super(null, destId);
        this.info = "(Confirmation)";
        this.edges = edges;
        this.color = Color.blue;
    }
}

class Wakeup extends Message {
    public Wakeup(Integer srcId, Integer destId) {
        super(srcId, destId);
        this.color = Color.white;
    }
}

class Connect extends Message {
    Integer edgeState;
    public Connect(Integer srcId, Integer destId, Integer level, Integer edgeState) {
        super(srcId, destId);
        this.info = "Connect - L" + level + ")";
        this.color = Color.black;
        this.level = level;
        this.edgeState = edgeState;
    }
}

class Initiate extends Message {
    String info = "(Initiate - ";
    Integer state;
    public Initiate(Integer srcId, Integer destId, Integer core, Integer level, Integer state) {
        super(srcId, destId);
        this.color = Color.green.darker();
        this.core = core;
        this.state = state;
        this.level = level;
        this.info = "(Initiate - C" + core + ", L" + level + ", ";
        if (level == 1) this.info += "SLEEPING)";
        else if (level == 2) this.info += "FIND)";
        else if (level == 3) this.info += "FOUND)";
    }
}

class Test extends Message {
    public Test(Integer srcId, Integer destId, Integer core, Integer level) {
        super(srcId, destId);
        this.core = core;
        this.level = level;
        this.info = "(Test - C" + core + ", L" + level + ")";
        this.color = Color.black;
    }
}

/*
class Inform extends Message {
    public Inform(Integer srcId, Integer destId, Integer core, Integer level) {
        super(srcId, destId, core, level, null);
        this.info += "Inform - " + core + ", " + level + ")";
        this.color = Color.green.darker();
    }
}
*/

class Report extends Message {
    Integer weight;
    public Report(Integer srcId, Integer destId, Integer weight) {
        super(srcId, destId);
        this.weight = weight;
        this.info = "(Report - " + weight + ")";
        this.color = Color.black;
    }
}

class Accept extends Message {
    public Accept(Integer srcId, Integer destId) {
        super(srcId, destId); 
        this.color = Color.green.darker();
        this.info = "(Accept)";
    }
}

class Reject extends Message {
    public Reject(Integer srcId, Integer destId) {
        super(srcId, destId); 
        this.color = Color.red;
        this.info = "(Reject)";
    }
}

class ChangeRoot extends Message {
    public ChangeRoot(Integer srcId, Integer destId) {
        super(srcId, destId); 
        this.color = Color.magenta.darker();
        this.info = "ChangeRoot";
    }
}
/*
class Push extends Message {
    Message message;
    public Push(Message message) {
        super(null, null, null, null, null);
        this.message = message;
    }
}
*/
class Done extends Message {
    public Done(Integer srcId, Integer destId) {
        super(srcId, destId); 
        this.color = Color.black;
    }
}