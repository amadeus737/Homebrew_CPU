using UnityEngine;
using System.Collections.Generic;

public class BitVals
{
    public int maxVal;
    public int bitshift;

    public BitVals(int mv, int bs)
    {
        maxVal = mv;
        bitshift = bs;
    }
}

public class Decoder : Rom
{
    private Dictionary<int, Value> _valueDictionary = new Dictionary<int, Value>();
    private Dictionary<int, BitVals> _bitValDictionary = new Dictionary<int, BitVals>();

    [SerializeField] private List<Value> _outputs = new List<Value>();
    
    public void RegisterInputs(int guid, int maxVal, int bitshift, Value v)
    {
        _valueDictionary[guid] = v;
        _bitValDictionary[guid] = new BitVals(maxVal, bitshift);
    }

    public int testval;

    protected override void Update()
    {
        base.Update();
        /*
        int address = 0;
        int bitwidth_prev = 0;
        foreach (int guid in _valueDictionary.Keys)
        {
            if (_valueDictionary[guid].state < lastValidData.state)
                lastValidData.state = _valueDictionary[guid].state;

            address |= (int)(((int)_valueDictionary[guid].value & (_bitValDictionary[guid].maxVal - 1)) << _bitValDictionary[guid].bitshift);
        }
    */    

        RequestData(testval);

        for (int i = 0; i < _outputs.Count; i++)
        {
            int v = (int)lastValidData.value;
            _outputs[i].value = (ulong)(v & (1 << i));
            _outputs[i].state = lastValidData.state;
        }
    }
}
