using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Bus")]
public class Bus : ScriptableObject
{
    [SerializeField] private Value _value;

    private Dictionary<int, Value> _valueDictionary = new Dictionary<int, Value>();

    public Value value { get { return _value; }}

    public void RegisterValue(int guid, Value data)
    {
        _valueDictionary[guid] = data;
        UpdateState();
    }

    public void UnregisterValue(int guid)
    {
        if (_valueDictionary.TryGetValue(guid, out Value v))
        {
            _valueDictionary.Remove(guid);
        }

        UpdateState();
    }

    public object Read()
    {
       return _value;

     
    }

    private void UpdateState()
    {
        int a = 0;
        foreach (int k in _valueDictionary.Keys)
            a = k;

        if (_valueDictionary.Count == 0) _value.state = ValueState.TriState;
        if (_valueDictionary.Count == 1) _value.state = _valueDictionary[a].state;
        if (_valueDictionary.Count > 1) _value.state = ValueState.Error;

        if (_value.state == ValueState.Good)
            _value.value = _valueDictionary[a].value;
    }
}
