using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class Rom : MonoBehaviour
{
    [SerializeField] private string _romName = "";
    [SerializeField] private float _delay = 10.0f;
    [SerializeField] private List<Value> _data = new List<Value>();
    [SerializeField] private ControlLine _outputEnable = null;
    [SerializeField] private Bus _outputBus = null;

    private int _size;
    private bool _dataValid = false;
    private bool _romLoaded = false;
    private float _requestTime;
    private float _elapsedTime;
    [SerializeField] private Value _lastValue;
    private int _lastAddress = 0;
    
    public int a;

    public bool valid { get { return _dataValid; } }
    public Value lastValidData { get { return _lastValue; } }

    public void RequestData(int address)
    {
        _elapsedTime = 0.0f;
        _dataValid = false;

        if (_data != null && _data.Count > 0)
        {
            _lastAddress = address;
        }
    }

    private void Start()
    {
        string rom_data = Application.dataPath + "/Roms/" + _romName + ".bin";
        FileStream fileStream = new FileStream(rom_data, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
        _size = (int)new FileInfo(rom_data).Length;
        using (BinaryReader binaryReader = new BinaryReader(fileStream))
        {
            for (int i = 0; i < _size; i++)
            {
                Value v = new Value();
                v.value = binaryReader.ReadByte();
                v.state = ValueState.Good;
                _data.Add(v);
            }
        }

        _lastValue = new Value();
        _lastValue.state = ValueState.Good;

        if (_outputEnable != null)
            _lastValue.state = _outputEnable.active ? ValueState.Good : ValueState.TriState;

        _romLoaded = true;
        fileStream.Close();
    }

    private void Update()
    {
        if (!_romLoaded)
            return;

        if (_dataValid)
        {
            _lastValue.value = _data[_lastAddress].value;
            _lastValue.state = ValueState.Good;

            if (_outputEnable != null)
                _lastValue.state = _outputEnable.active ? ValueState.Good : ValueState.TriState;
        }

        if (_outputBus != null)
        {
            if (_outputEnable == null)
                _outputBus.RegisterValue(this.GetInstanceID(), _lastValue);
            else
            {
                if (_outputEnable.active)
                    _outputBus.RegisterValue(this.GetInstanceID(), _lastValue);
                else
                    _outputBus.UnregisterValue(this.GetInstanceID());
            }
        }

        if (_dataValid) return;

        _elapsedTime += Time.deltaTime;
        if (_elapsedTime >= _delay)
        {
            _dataValid = true;
        }
    }
}