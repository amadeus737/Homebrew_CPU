using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Clock : MonoBehaviour
{
    private static Clock _instance = null;
    public static Clock Instance()
    {
        if (_instance == null)
            _instance = FindObjectOfType<Clock>();

        return _instance;
    }

    [SerializeField] private Signal _signal = null;
    [SerializeField] private SignalState _state = SignalState.None;
    [SerializeField] private ClockMode _mode = ClockMode.None;
    [SerializeField] private float _rate = 1.0f; // Hertz
    [Range(0.0f, 1.0f)]
    [SerializeField] private float _edge_transition_time_ratio = 0.2f;

    public ClockMode mode { get { return _mode; } }
    public bool halted { get { return _isHalted; } }
    public float rate { get { return _rate; } }
    public float period { get { return _cycle_period; } }
    public float halfPeriod { get { return _half_cycle_period; } }
    public float edge_ratio { get { return _edge_transition_time_ratio; }}

    private bool _isHalted = false;
    private float _elapsed_time = 0.0f;
    private float _cycle_period;
    private float _half_cycle_period;
    private Rom _programrom;
    private int c;

    private void Start()
    {
        _state = _signal.state;
        _mode = ClockMode.Step;
        _isHalted = true;
        _elapsed_time = 0.0f;

        _programrom = GetComponent<Rom>();
        c = 2;
    }

    private void Update()
    {
        _state = _signal.state;
        _cycle_period = 1.0f / _rate;
        _half_cycle_period = 0.5f * _cycle_period;

        if (_isHalted || _mode != ClockMode.Run)
            return;

        Count(Time.deltaTime);
    }

    public void SetMode(ClockMode m)
    {
        _mode = m;
    }

    public void Step()
    {
        _isHalted = false;
        Count(_half_cycle_period);
    }

    public void Halt()
    {
        _mode = ClockMode.Step;
        _isHalted = true;
    }

    public void Resume()
    {
        _mode = ClockMode.Run;
        _isHalted = false;
    }

    private void Count(float deltaTime)
    {
        _elapsed_time += deltaTime;

        if (_elapsed_time >= 0.97f * _half_cycle_period)
        {
            _signal.ToggleState();
            _elapsed_time = 0;
        }

        if (_programrom)
            _programrom.RequestData(c);
        c++;
    }
}