# Fundamentals of Data Analysis — Open-Book Exam Reference

**Course:** Fundamentals of Data Analysis | **Topics covered:** Topic 1 (Course Overview & Basic Statistical Processes) + Topic 2 (Summarising Data & Probability)
**Sources used:** Topic 1 & Topic 2 lecture slides, *Mind on Statistics* Chapters 1–3 (textbook), Lab Topic 1 & Lab Topic 2 sheets, Topic 1 textbook exercise set, 40-question Practice MCQ set.

## How to use this document
- Ctrl+F / Cmd+F for a keyword — every section is labelled and terms are **bold** on first definition.
- ⚠️ = exam trap. → = cause/effect or "leads to". ⭐ = heavily tested (appears in MCQs/lecture emphasis repeatedly).
- Jump to **Part D (⚡ Rapid Reference)** at the very end for a 30-second last-look page.
- ⚠️ CONFLICT flags note where sources phrase something differently.

---

# 🧮 MASTER FORMULA & EXCEL FUNCTION SHEET

## Descriptive statistics formulas

**Sample mean (x̄)**
- Formula: x̄ = (Σx) / n
- Variables: Σx = sum of all data values; n = sample size
- Use when: summarising the "average" of quantitative data; data are roughly symmetric (no strong skew, no big outliers)
- Watch out for: heavily pulled by outliers/skew — an outlier of 1200mm in a fish-length dataset shifted the mean from 262.67 to 278.8
- Units: same as original data
- Excel: `=AVERAGE(range)`

**Sample median**
- Formula: middle value when data sorted ascending. Odd n → the single middle value. Even n → average of the two middle values.
- Use when: data are skewed or contain outliers (resistant/robust measure)
- Watch out for: with n even, need average of two middle observations, not just one
- Excel: `=MEDIAN(range)` or `=QUARTILE(range,2)` or `=PERCENTILE(range,0.5)` (all equivalent)
- Example: 58 fish lengths (even n) → median = average of 29th (250) and 30th (260) ordered values = 255

**Range**
- Formula: Range = Maximum − Minimum
- Use when: quick, crude spread measure
- Watch out for: extremely sensitive to outliers/extremes (uses only 2 points)
- Excel: `=MAX(range)-MIN(range)`

**Quartiles (Q1, Q3) — "by hand" method**
- Q1 = median of the lower half of the data (below the overall median); Q3 = median of the upper half
- ⚠️ CONFLICT: the "by-hand" method and software (Excel) algorithms can give **slightly different answers** — don't panic if your hand calc is close but not identical to Excel's
- Excel: `=QUARTILE(range,1)` for Q1, `=QUARTILE(range,3)` for Q3 (`=QUARTILE(range,2)`=median); equivalently `=PERCENTILE(range,0.25)` and `=PERCENTILE(range,0.75)`
- Worked example: fish data (reef shark removed, n=57): median=250 (29th obs); Q1 = between 14th/15th obs of lower half; Q3 between 290 & 305 = 297.5; IQR = 297.5−237.5 = 60; Range = 410−85 = 325mm

**Interquartile range (IQR)**
- Formula: IQR = Q3 − Q1
- Meaning: range of the **middle 50%** of the data
- Use when: measuring spread while resisting outliers/skew (unlike range, variance, SD)
- Watch out for: only uses the middle half — ignores tails entirely
- Example: Q1=$1,800, Q3=$3,100 → IQR = $1,300
- Excel: `=QUARTILE(range,3)-QUARTILE(range,1)`

**Sample variance / standard deviation**
- Meaning: variance = average squared distance of observations from the mean ("on average, how far are the observations spread out from the mean?"); SD = √variance
- Why square root? Variance's units are squared (e.g. $²), which isn't practically interpretable; SD restores the original units ($)
- Course note: this course calculates variance/SD **only via Excel**, not by hand formula
- Units: SD has the same units as the original data; variance has squared units
- Excel: `=STDEV(range)` or `=STDEV.S(range)` for **sample** SD
- Key facts: identical values in a dataset → SD = 0. Adding the same constant to every value leaves SD **unchanged** (shifts the mean but not the spread). Multiplying every value by a constant *k* scales SD by |k| (not directly quizzed in sources, but consistent with the "adding" rule tested).

**Empirical Rule** (bell-shaped / Normal-like distributions only)
- 68% of values lie within **1** SD of the mean: [μ−σ, μ+σ]
- 95% of values lie within **2** SD of the mean: [μ−2σ, μ+2σ]
- 99.7% of values lie within **3** SD of the mean: [μ−3σ, μ+3σ]
- μ = "mu" = mean; σ = "sigma" = standard deviation
- Use when: distribution is bell-shaped/symmetric (check with a histogram first)
- Watch out for: invalid for skewed/non-bell-shaped data
- Worked example: sleep hours, μ=7, σ=1.7 → 68% sleep between 5.3–8.7 hrs; 95% between 3.6–10.4 hrs; 99.7% between 1.9–12.1 hrs
- Worked example: component diameter μ=50mm, σ=2mm. A 46mm part is 2 SD below mean → only ~2.5% of parts are smaller (i.e., (100%−95%)/2). A 56mm part is 3 SD above → only ~0.15% are larger ((100%−99.7%)/2)
- **Tail-probability shortcut**: below −2σ ≈ 2.5%; above +2σ ≈ 2.5%; below −3σ ≈ 0.15%; above +3σ ≈ 0.15% (each tail is the leftover outside the rule, halved)

**Margin of error (survey proportions)**
- Approximate/conservative formula: margin of error ≈ 1/√n (as a proportion), or (1/√n)×100%
- Example: n=1500 → margin of error ≈ ±3% (a very common polling benchmark)
- Rule: to **halve** the margin of error, you need to **quadruple** (4×) the sample size (n=2500→10,000 takes MoE from ~2% to ~1%)
- Watch out for: this is for estimating a single population **proportion**; comparing two proportions has a *larger* margin of error than either individually

**Relative frequency / percentage**
- Formula: relative frequency = (count in category) / (total count); percentage = relative frequency × 100
- ⚠️ Percentages in a pie chart **must sum to 100%** — if they don't, the categories/data have an error, or the underlying variable allows multiple responses per subject (e.g., a "select all that apply" survey question)

## Probability formulas

**Complement rule**
- P(not A) = 1 − P(A)
- Example: P(defective)=0.08 → P(not defective) = 0.92

**"At least one" rule**
- P(at least one event occurs) = 1 − P(no events occur)
- Example: P(rain on ≥1 day next week) = 1 − P(no rain all week)

**Sum of all probabilities**
- All possible outcomes' probabilities must sum to **exactly 1**; every individual probability must be between 0 and 1 inclusive (0 ≤ P ≤ 1)

**Empirical (estimated) probability**
- Formula: estimated P(event) = (number of times event occurred) / (total number of trials/observations)
- Example: 18 late out of 120 deliveries → estimated P(late) = 18/120 = 0.15

**Expected value (mean) of a discrete random variable**
- Formula: E(X) = μ = Σ [x·P(x)] (sum of each outcome value × its probability)
- Meaning: the **long-run average** outcome over many repetitions — NOT a guarantee for any single instance
- Watch out for: E(X) need not be one of the possible values of X, and does not mean every trial yields that result
- Worked example (orders/hour): E(X) = 0(0.10)+1(0.20)+2(0.40)+3(0.30) = 0+0.2+0.8+0.9 = **1.9**
- Worked example (investment): 0.5(20,000)+0.3(50,000)+0.2(−10,000) = 10,000+15,000−2,000 = **$23,000**
- Worked example (insurance): 0.90(0)+0.08(5,000)+0.02(20,000) = 0+400+400 = **$800** expected claim cost

**Variance / SD of a random variable**
- Notation: Var(X) or σ² (variance); σ (SD, "sigma"), same units as X
- Meaning: spread of the probability distribution around its mean

## Excel function quick-reference (from Lab 1 & Lab 2)

| Task | Excel formula | Notes |
|---|---|---|
| Mean | `=AVERAGE(range)` | |
| Median | `=MEDIAN(range)` | = `QUARTILE(range,2)` = `PERCENTILE(range,0.5)` |
| Min / Max | `=MIN(range)` / `=MAX(range)` | |
| Range | `=MAX(range)-MIN(range)` | manual subtraction of two cells |
| Lower quartile (Q1) | `=QUARTILE(range,1)` | = `PERCENTILE(range,0.25)` |
| Upper quartile (Q3) | `=QUARTILE(range,3)` | = `PERCENTILE(range,0.75)` |
| IQR | `=Q3 cell − Q1 cell` | e.g. `=F7-F6` |
| Sample SD | `=STDEV(range)` or `=STDEV.S(range)` | this course: SD always via Excel, never hand-calculated |
| Count matching ONE condition | `=COUNTIF(range,criteria)` | criteria can be a value, e.g. `1`, or comparison string `">72.86"` |
| Count matching MULTIPLE conditions (AND) | `=COUNTIFS(range1,crit1,range2,crit2,...)` | e.g. count males from Vic: `=COUNTIFS($A$2:$A$53,1,$B$2:$B$53,1)` |
| Estimate empirical probability from data | `=COUNTIFS(...)/n` | e.g. `=E44/85` for P(within 1 SD) |

**Relative vs absolute cell references** ⭐ (tested in Lab 1, easy exam trap)
- **Relative reference** (e.g. `E2:E53`): adjusts automatically when the formula is copied/dragged to a new cell (row/column offsets shift)
- **Absolute reference** (e.g. `$B$2:$B$53`): stays fixed on the same cells no matter where the formula is copied — denoted with `$` before the column letter and/or row number
- **When to use absolute**: whenever a formula must keep pointing at the *same* fixed cell/range while being copied elsewhere — e.g. `COUNTIF($B$2:$B$53,1)` copied down a list of states must always scan the *same* full data range, only the criteria changes
- ⚠️ EXAM TRAP: forgetting `$` on a reference that should be absolute → the range silently shifts when copied, giving wrong counts

**Pivot tables** (Lab 1, Quest 3)
1. Click any cell in the data range → *Insert > PivotTable* → confirm full data range selected → place in new worksheet
2. **One-way table**: drag one field (e.g. *Type*) to **both** Rows and Values → gives a frequency count per category
3. **Two-way table**: drag one field (e.g. *Type*) to Rows, a second field (e.g. *Sex*) to Columns, and either field to Values → gives a cross-tabulated count (contingency table)
4. To convert pivot counts into % of total: copy the counts as **values** elsewhere (pivot cells can't be used directly in formulas), then divide each by the grand total × 100 using an **absolute** reference to the total cell

**Making a histogram in Excel** (Lab 1 Quest 4 / Lab 2 Quest 2)
1. Decide bin **upper bounds** so you get ~10 bins: bin width ≈ range(max−min)/10, rounded to a convenient number
2. Type the bin upper bounds into a column (e.g. 10, 20, 30 … up to a value ≥ max)
3. *Data > Data Analysis > Histogram* → set Input Range (the raw data) and Bin Range (your bin values, include header, tick "Labels") → tick **Chart Output**
4. Remove gaps between bars: right-click a bar → *Format Data Series* → Gap Width → **0%** (histograms should have touching bars; only categorical bar charts have gaps)

---

# PART A — TOPIC 1: Data Collection, Study Design & Intro to Graphs

## A1. Core Concepts: What Is Statistics & the Data Investigation Cycle

- **Statistics**: the "science of learning from data"; the study of **variation** and **uncertainty**; used to obtain, understand, and interpret data in any discipline with quantitative information
- **Data**: facts (often numerical) collected together for reference
- Chain: **Data → Statistics → Knowledge**
- **The discovery-of-knowledge process** (a.k.a. the **PPDAC cycle** — Problem, Plan, Data, Analyse, Conclusions — MacKay & Oldfield 1994; Wild & Pfannkuch 1999), 5 steps:
  1. Identify issues/questions (Issues)
  2. Plan and carry out collection of useful data (Planning) — includes what is/isn't available, how to obtain representative data, choice of measures
  3. Explore data with visualisations and summaries (Collecting/handling → Exploring/analysing)
  4. Choose/develop appropriate probability models and analyse using standard/adapted/new techniques
  5. Interpret models & analysis in context; communicate results (Interpreting in context)
  - Diagram is a **cycle**: Issues → Planning → Collecting,handling → Exploring,analysing → Interpreting in context → (back to Issues)
  - "C" sometimes stands for "Conclusions", sometimes "Comment in context" — both emphasise interpretation/communication
- **Topic 1 (this course) focuses on the first 3 parts of the cycle**: Issues → Planning → Collecting the data. Topic 2 moves into Exploring/Analysing (summaries, probability).
- Key terms: **evidence-based** = decisions/procedures using objective evidence rather than anecdote/opinion; **quantitative information** = information from obtaining/understanding/interpreting/using data; **uncertainty** = some form of unpredictability; **variation** = observations not all one value

## A2. Datasets, Variables & Observations — Key Terms

| Term | Meaning |
|---|---|
| **Raw data** | numbers/category labels collected but **not yet processed** in any way |
| **Variable** | a trait/characteristic being measured that differs from individual to individual (e.g. age, height, superpower preference) |
| **Observation** | a single piece of data collected for one variable on one unit/subject (e.g. "age = 18" is one observation); can also mean all responses for one unit |
| **Dataset** | the **complete set** of raw data, for **all** units and **all** variables |
| **Sample size (n)** | total number of units/observations in the dataset |
| **Observational/experimental unit / subject** | the individual, object, or entity on which observations are made |
| **Primary data** | data collected by the investigators themselves |
| **Secondary data** | data collected by others, used by the investigator |
| **Identifier** | a code assigned to a subject/item/person |

⚠️ EXAM TRAP: a dataset **without an identifier column** can become impossible to interpret correctly if rows get reordered or duplicated (textbook Example 1.2 — a household water-usage dataset had no identifier and ambiguous year/quarter ordering, making it unreliable).

## A3. ⭐ Types of Data / Variables — THE classification tree (heavily tested)

**Two master dichotomies apply to every variable:**

1. **Qualitative (= categorical)** vs **Quantitative**
2. **Discrete** vs **Continuous**

```
                 Variable
              /            \
      Categorical        Quantitative
      (Qualitative)     
       /       \          /        \
   Nominal   Ordinal   Discrete   Continuous
                        (Count)   
                                    /      \
                                Interval   Ratio
```
(Categorical variables are always discrete in the sense of taking distinct category labels; "Discrete" as its own branch in the textbook figure specifically covers **Count** data, e.g. number of children.)

| Type | Definition | Examples |
|---|---|---|
| **Categorical (qualitative)** | raw data are category names/labels; each individual falls into exactly one category | gender, hair colour, degree program, favourite football team |
| — **Nominal** | categories have **no meaningful order** | state of origin, degree program (Accounting/Marketing/Finance…), hair colour |
| — **Ordinal** | categories have a **meaningful order/ranking** | driving-skill rating (better/average/worse than average), military rank, satisfaction scale (1=very dissatisfied…5=very satisfied), grade (HD/D/C/P) |
| **Quantitative** | raw data are numbers that are **meaningful as numbers** (not just codes) | height, weight, age, income |
| — **Discrete** | takes individual/separated values from a **countable** set, incl. **count data** (number of items/events/people) | number of children in a family, number of accidents at an intersection/year, number of DVDs owned |
| — **Continuous** | takes any value within an interval; typically rounded for recording ("to the nearest…") | height, weight, temperature, time |
| — — **Interval** (subtype of continuous) | equal intervals are meaningful but **no true/meaningful zero** | temperature in °C (0°C ≠ "no heat"); contentment rating |
| — — **Ratio** (subtype of continuous) | equal intervals **and** a **true zero**, so ratios between values are meaningful | length, distance, weight, most physical/engineering measurements |

**The Likert scale**: the specific 1-to-5 (or 1-to-7) ordinal scale used in questionnaires ("strongly disagree"→"strongly agree"). Numeric codes on a Likert scale should be **equidistant from a central neutral point** (e.g., for 1–7, code 4 = neutral, 3&5 equidistant, 2&6 equidistant, 1&7 equidistant) if you intend to average it.

⚠️ EXAM TRAPS (variable classification — this is the single most repeated MCQ theme across all sources):
- **Numbers that are really codes are NOT quantitative** — e.g. student ID numbers, or 1=Male/2=Female codes. "MIND YOUR STEP": *data given in terms of numbers are not necessarily quantitative.*
- **Whole-number data are NOT automatically count data** — e.g. age recorded in whole years is still fundamentally **continuous**, just rounded for recording. "MIND YOUR STEP": *data recorded in whole numbers are not necessarily count data.*
- **Money** is continuous (has decimal places), even though it's often *treated* as though discrete in casual conversation.
- **Time** is continuous no matter the units used (even though we're used to counting "days").
- A quantitative variable can be **deliberately categorised** for analysis purposes (e.g. grouping ages into "young/middle/old") — this is a valid choice, but you lose information; don't do this to a quantitative variable "without good reason" if the raw values are available.
- **Number of customers per hour** = **discrete numerical** (count), NOT continuous (Practice MCQ Q4).
- **Exact distance travelled (km)** = **continuous numerical** (Practice MCQ Q5), even though "km" might sound countable.
- **Degree program** (no natural order) = **nominal categorical** (Practice MCQ Q6).
- **Satisfaction rating 1–5** = **ordinal categorical**, not quantitative — even though it's coded with numbers! (Practice MCQ Q7)

**"What to look for" keyword cues:**
- Sees "*category/type/label with no order*" → nominal
- Sees "*rating scale, rank, level of agreement*" → ordinal
- Sees "*count of, number of ___s*" → discrete (count)
- Sees "*measured in cm/kg/seconds/$, to the nearest…*" → continuous
- Sees "*has a true zero / ratios make sense*" → ratio; "*zero doesn't mean absence*" (e.g. °C, dates) → interval

## A4. Types of Investigations: Observational Study vs Experiment vs Survey

| Type | Definition | Key feature | Example |
|---|---|---|---|
| **Observational study** | investigators **observe** units/subjects without controlling or altering their behaviour | no manipulation of conditions | whether spending patterns are associated with time of day visited |
| **Experiment** | investigators **control/manipulate** conditions (apply "treatments") and measure the response | deliberate manipulation of explanatory variable(s) | whether a discount **causes** increased spending |
| **Survey** | asking questions of subjects/units, with data being the responses | subjects are usually people, asked directly | customer opinions, preferences, attitudes |

- A **key element of every type** is **randomisation** of units/subjects (random selection for surveys, random allocation for experiments, random representativeness assumed for observational studies).
- Experiments may be further split into **laboratory** vs **field** experiments (field = outside a lab).
- Many real investigations are a **mixture** of types (e.g. Example 2.3, brown vs white eggs: colour/brand/housing are *observed* [observational], but load/temperature/etc. were *controlled* [experimental] — a mixture).

⚠️ EXAM TRAP: **Only a randomised experiment can properly establish cause-and-effect.** Observational studies can show **association/correlation** but cannot, by themselves, prove causation (see A14 — Confounding).

## A5. Response vs Explanatory Variables

| Term | Also known as | Meaning |
|---|---|---|
| **Response variable** | dependent variable, outcome variable | the outcome(s) of interest — what you're trying to explain/measure the effect on |
| **Explanatory variable** | predictor variable, independent variable | variable(s) chosen to investigate their effect on the response |

- Example: washing-powder brand & water temperature (explanatory) → amount of dirt removed (response)
- Example: discount (explanatory) → spending (response)
- Example: number of push-ups & incline level (explanatory) → pulse rate (response)

⚠️ EXAM TRAP (explicitly flagged in the textbook as a "Mind your step"): **Labelling a variable "explanatory" and another "response" does NOT by itself imply a causal relationship is proven** — it only implies that knowledge of the explanatory variable's value may help predict/inform the response variable's behaviour. Causation requires a properly randomised experiment (see A14).

**Worked ID practice (from Topic1TextQuest exercises)** — pattern: whichever variable is being predicted/measured as an outcome is the **response**; the other is **explanatory**:
- Weight & fitness-test performance → response = fitness performance; explanatory = weight
- Feelings about public transport importance & age → response = feelings about transport; explanatory = age
- Opinion on PM (favour/not) & gender → response = opinion; explanatory = gender
- Daughter's height & mother's height → response = daughter's height; explanatory = mother's height

## A6. Planning a Data Investigation

Planning must involve (checklist — order roughly reflects the process):
1. Identifying the **topic(s)/issue(s)** of interest
2. Identifying the **variables** to be observed
3. Identifying the **subjects/units** of the study
4. Planning the **practicalities**
5. Planning collection of **representative** data
6. Carrying out a **pilot study** (preliminary trial) to check feasibility — "the answer to 'should we do a pilot study' is almost always yes"

**Practical questions to ask when planning** (from textbook "Keep in Mind" boxes):
- What do we want to find out? What can we find out? What can we observe/measure? Can we measure what we want?
- Is there anything else we should observe/record in case it's useful? (If it "costs" no more to record, the answer is yes.)
- **Validity of measures is crucial** — are we measuring what we actually want to measure? (e.g. using "likelihood to recommend" as a proxy for "satisfaction" — is that valid?)
- Always report **how, when, and where** data were collected, in enough detail to permit interpretation/replication (textbook "Keep in Mind" #1)
- Data can only be used to infer to a larger group if the sample is **randomly representative with respect to the question(s) of interest** (textbook "Keep in Mind" #2)

⚠️ EXAM TRAP: measurement **precision** matters — Example 1.1 in the textbook: students recorded river pH only to the nearest whole number, which was too coarse to detect real variation — the data collection method itself made the study unable to answer the research question, regardless of good planning otherwise.

## A7. Sampling Fundamentals

| Term | Meaning |
|---|---|
| **Population** | the larger group about which inferences are to be made |
| **Sample** | the smaller group actually measured, drawn from the population |
| **Census** | when measurements are taken on the **whole** population (sample = population) |
| **Sample data** | data considered randomly representative of a larger group/situation |
| **Simple random sample (SRS)** | every unit in the population has the **same** (equal) probability/chance of being selected; every conceivable group of the required sample size has the same chance of being the chosen sample |
| **Probability sampling plan** | any sampling plan where everyone in the population has a **specified (known)** chance of selection |

- A census is "ideal" for accuracy but usually **not achievable** (cost/time) — e.g. the Australian census costs hundreds of millions of dollars and happens only every 5 years.
- To build an SRS: need a **list of the units in the population** + a source of **random numbers** (tables of random digits, software, or physical methods like drawing from a hat / lottery balls).

⚠️ CASE STUDY 2.1 — **Literary Digest Poll (1936) vs Gallup**: Literary Digest mailed 10 million surveys (from magazine subscriber/car owner/phone directory lists — an unrepresentative, wealthier sampling frame), got 2.3 million responses (23% response rate, mostly from people motivated to complain/change) → wrongly predicted Landon would beat Roosevelt. Gallup instead used a **random sample of just 3,000** people and predicted the *Digest's* result to within 1%, and correctly called the actual election. **Lesson: sample size alone doesn't guarantee accuracy — random representativeness is what matters.** ⭐ Classic exam example of "bigger biased sample loses to smaller random sample."

## A8. Other Sampling Methods — comparison table

| Method | How it works | When/why used | Key distinction from the others |
|---|---|---|---|
| **Simple random sample (SRS)** | every unit has equal chance of selection, chosen via random numbers | baseline "gold standard"; simplest to analyse | — |
| **Stratified sampling** | population divided into **strata** (identifiable subgroups, e.g. by region or gender) → take a **separate SRS from each stratum** | ensures each important subgroup is properly represented; increases precision when strata differ from each other | population divided into a **few large**, meaningful strata; sample taken **from each** stratum |
| **Cluster sampling** | population divided into **clusters** (e.g. suburbs, city blocks) → **randomly select some whole clusters**, then sample all/some units within only those selected clusters | practical when no full list of individuals exists, only a list of clusters (e.g. list of flights, not passengers) | population divided into **many small** clusters; only **some** clusters (not all) are used, but each used cluster ~fully sampled |
| **Multistage sampling** | combination of methods applied in stages (e.g. stratify by region → cluster-sample suburbs within region → SRS households within suburb) | large surveys with complex populations | multi-step combination of the above |

**Beer example (Topic 1 slides) — SRS vs stratified**: class of 80 (60 men, 20 women); want sample of 8.
- SRS: each of 80 has 1-in-10 chance; could randomly get 0–8 women → results **vary a lot** by chance, and might badly under/over-represent women
- Stratified: split into strata (men, women) first, then take proportional SRS from each — 6 men + 2 women (i.e. 1-in-10 from **each** stratum) → same overall 1-in-10 chance per person, but sample **composition matches the population exactly**

⚠️ EXAM TRAP: **Cluster sampling is often confused with stratified sampling** — the key test: in stratified sampling you sample **within every stratum**; in cluster sampling you only visit **some** of the clusters (but sample thoroughly within those chosen).

## A9. Bias in Surveys & Sampling Disasters

**Three core types of bias** ⭐:

| Bias type | Definition | Example |
|---|---|---|
| **Selection bias** | the method for selecting participants produces a sample that does **not represent** the population of interest | surveying only university students to gauge attitudes about raising *university* fees represents students, not the general public; surveying customers still sitting in a restaurant (excludes those who left) |
| **Nonresponse bias** (nonparticipation bias) | a representative sample is chosen, but a **subset doesn't respond/can't be contacted**, and non-responders differ systematically from responders | phoning people at home in early evening excludes those working then; mail surveys under-represent people who don't reply |
| **Response bias** | participants give **incorrect/untruthful information**, due to question wording, interviewer behaviour, sensitive topics, desire to please, etc. | asking about smoking/drinking (people understate); "Aren't our excellent products reasonably priced?" (leading wording) |

**Sampling disasters / other bias sources:**

| Problem | What it is |
|---|---|
| **Wrong sampling frame** (undercoverage) | the *list* used to select from doesn't cover the whole population of interest, e.g. a phone directory excludes unlisted numbers, mobile-only households |
| **Not reaching individuals selected** | can't contact the specific person chosen (must persist, not substitute with the "next easiest" person — that reintroduces bias) |
| **Self-selected sample** | volunteers choose themselves to participate (e.g. call-in polls, website polls) — **usually close to worthless**, because only people with strong opinions bother to respond |
| **Convenience / haphazard sample** | sample chosen simply because it's easy to get (not randomly) |

⚠️ EXAM TRAP: **"random" ≠ "haphazard"** — a "quickie poll" of whoever answers the phone during a 2-hour window is NOT randomly representative of an area's whole population (misses people not home then).

**Practicing bias identification** (from Topic1TextQuest exercises) — always ask "does the *method* systematically produce a sample unlike the population (selection), does a *subgroup* fail to respond (nonresponse), or do people give *untrue answers* (response)?":
- University's most expensive campus café, lunch hour only → **selection bias** (misses students who don't eat there / can't afford it / eat then) — this is Practice MCQ Q1's issue, framed as "sample may not represent the population."
- "How responsible are you with your finances?" → **response bias** (socially desirable answers) — Practice MCQ Q2.
- 500 residences surveyed, 200 refuse → **nonresponse bias**.

## A10. Margin of Error & Sample Size

- **Margin of error** = how far away an estimate is likely to be from the true population value; ± the margin, we're confident the true % falls within that range
- Rule of thumb: n≈1000–1500 gives margin of error **≈ ±3%** — this is why national polls often use ~1000–1500 respondents even though the population is millions
- Formula: margin of error ≈ 1/√n (as a %, ×100)
- **Bigger sample ⇒ smaller margin of error**, but the relationship isn't linear: **quadrupling n only halves the margin of error** (n: 2500→10,000 takes MoE from 2% to 1%)
- Margin of error describes the uncertainty in estimating a **proportion**; comparing two subgroup proportions, or looking at small subgroups, needs an **even larger** sample for the same precision
- Nielsen TV ratings and similar are based on samples of only a few thousand households, much cheaper/faster than a census, though data quality can differ (fewer interviewers needed → potentially *better* controlled procedures, ironically)

## A11. Survey Question Design

**Open vs closed questions**

| Type | Definition | Pros | Cons |
|---|---|---|---|
| **Open question** | respondent answers in own words | captures unanticipated answers; nothing is missed | hard to summarise/categorise, especially at scale |
| **Closed question** | respondent picks from a given list (often incl. "other") | easy to analyse/compare | may not include the option the respondent would have said; can push respondents toward listed options (closed-choice answers can differ sharply from open-response answers to the same underlying question) |

- **Compromise approach**: run a small pilot with an open question to find common answers, then build a closed-question list from the most common responses.

**Common wording/design pitfalls** — ⚠️ EXAM TRAPS:
- **Deliberate (leading) bias**: wording designed to push a particular answer — watch for phrases like *"Do you agree that…?"*
- **Ambiguity/unintentional bias**: vague terms mean different things to different people (e.g. "drugs" — prescription? illegal? caffeine?)
- **Desire of respondents to please**: people often answer to please the interviewer/researcher, or to appear more socially acceptable
- **Asking the uninformed**: respondents may answer confidently about things they don't actually know/understand
- **Unnecessarily complex or double-barrelled questions**: asking two things at once (e.g. "Shouldn't former drug dealers not be allowed to work in hospitals after release from prison?" — confusing double negative + two questions in one)
- **Ordering of questions**: earlier questions can prime/anchor responses to later ones (order effects)
- **Confidentiality vs anonymity**: *confidentiality* = researcher knows identities but promises not to release them; *anonymity* = researcher **never knows** identities at all — anonymity increases honest responses on sensitive topics but is a stronger, harder-to-guarantee promise
- ⚠️ CASE STUDY 2.3 (NZ anti-smacking referendum): *"Should a smack as part of good parental correction be a criminal offence?"* — the word **"good" before "parental correction" is a leading value-judgment** baked into the question, priming a "no" answer

## A12. Designing Experiments

| Term | Meaning |
|---|---|
| **Randomised experiment** | subjects are **randomly assigned** to receive a specific treatment (or to a control group) |
| **Control group** | treated identically to other groups **except** they don't receive the active treatment being tested — provides the "what would have happened anyway" baseline |
| **Placebo** | a fake treatment that looks like the real one but has **no active ingredient**; controls for the *placebo effect* (people responding just because they believe they're being treated) |
| **Blinding** | preventing participants and/or researchers from knowing which treatment was given, to avoid bias |
| — **Single-blind** | only ONE party (either the participant OR the researcher, not both) doesn't know the treatment |
| — **Double-blind** | **NEITHER** the participant nor the researcher taking measurements knows who had which treatment — the gold standard, avoids both subject bias and experimenter/measurement bias |
| **Double dummy** | when comparing two *active* treatments with different forms (e.g. a patch vs a gum) that can't be disguised as each other, give **every participant both a real dose of one treatment and a placebo of the other form**, so nobody can tell which active treatment they're on |
| **Matched-pair design** | subjects matched into pairs on relevant traits (age, IQ, weight…), then **one member of each pair randomly gets treatment A, the other gets treatment B** — a special case of a **block design** with exactly 2 treatments |
| **Block design** | experimental units divided into homogeneous groups (**blocks**), and each treatment is randomly assigned to one or more units *within* each block — used to control a **known** source of variability |
| **Repeated measures design** | in the social sciences, the *same* participants are measured repeatedly under different treatment conditions (their own results become the "block") |
| **Random order of treatments** | when the *same* unit receives multiple treatments in sequence, the **order** must be randomised too (to avoid learning/fatigue/practice effects contaminating later treatments) |

**Why randomise treatment assignment?**
- Randomisation makes the groups **representative of the same underlying conditions** in all respects **except** the explanatory variable(s) being manipulated → lets you attribute response differences to the treatment (cause-and-effect), assuming the difference is bigger than natural chance variation
- Randomisation also protects against **hidden/unknown biasing variables** the experimenter didn't even think to control for

**Worked example (caffeine & swimming speed, n=50 available)** — two valid designs:
- *Completely randomised design*: randomly assign 25 to caffeine, 25 to placebo; compare the two groups' average times
- *Matched-pair design* — Method 1: each of the 50 swimmers does BOTH treatments a week apart (randomise which treatment first), take each swimmer's own time difference
- *Matched-pair design* — Method 2: pair up swimmers by similar speed (fastest 2 = pair 1, etc.), randomly assign one member of each pair to caffeine and the other to placebo, analyse the **within-pair difference**

⚠️ EXAM TRAP: **No matter how many explanatory variables an experiment has, treatment assignment must always be random** once the (possible) explanatory variables are accounted for.

## A13. Observational Study Subtypes

| Type | Definition | Notes |
|---|---|---|
| **Retrospective study** | data are from the **past** — participants recall past events, OR researchers use already-recorded information (e.g. medical records) | risk: people don't remember past events accurately |
| **Prospective study** | researchers follow participants **forward** in time, recording events/variables as they happen | generally the **better** procedure — avoids recall bias — but participants may change behaviour *because* they know they're being observed/recorded (relates to Hawthorne effect) |
| **Case-control study** | "cases" (who have a particular attribute/condition, e.g. a disease) are compared to "controls" (who don't), on explanatory variables of interest | good for **rare** outcomes (e.g. rare disease) where a randomised experiment or long prospective study would be impractical, unethical, unaffordable, or too slow |

**Advantages of case-control studies**: avoid ethical issues of experimentally assigning harmful/beneficial treatments; efficient for time/money and for studying rare outcomes (an experiment might wait years and still get too few cases for valid comparison). Controls should be chosen to be as similar as possible to cases except for the disease/outcome status (e.g. choosing hospitalised patients with *other* conditions as controls can help account for general health/access-to-care as a confounder).

## A14. Cautions in Experiments & Observational Studies

| Issue | Definition | Example |
|---|---|---|
| **Confounding variable** | a variable associated with the explanatory variable that could also explain (part of) the effect on the response — makes it impossible to separate the "true" effect of the explanatory variable | baldness & heart attack risk in men — age is confounded with both baldness and heart-attack risk |
| **Lurking / hidden variable** | a variable that has an effect but **was not considered/measured** at all — same general idea as confounding, term often used when the variable wasn't even in the original design | sanitation level lurking behind a (spurious) breastfeeding-vs-infant-mortality relationship |
| **Interacting variables** | the effect of one explanatory variable on the response **depends on the level of another** explanatory variable | detergent A vs B might tie on one stain type but A clearly beats B on another stain type — the "best detergent" answer depends on stain type |
| **Extending results inappropriately** | generalising study conclusions beyond the population/situation the (random) sample was actually drawn from | convenience samples/volunteers shouldn't be assumed representative of a wider group without justification |
| **Hawthorne effect** | participants behave **differently just because they know they're in a study**, regardless of which treatment they got (first identified at the Western Electric Hawthorne plant, 1920s) | factory workers worked harder simply from being observed, not from the actual condition tested |
| **Experimenter effect** | the researcher(s) unintentionally bias results — e.g. treating subjects differently based on which treatment they're getting, or subtly signalling the "desired" outcome | Example 2.9 "Dull rats": experimenters told (falsely) that some rats were bred "maze bright" got better results from those rats than experimenters told their rats were "maze dull" — same rats, different *experimenter* beliefs |
| **Ecological validity** | whether an experiment's artificial/laboratory setting still reflects real-world behaviour | role-playing "saying no to requests" in a lab may not predict real assertiveness, since role-play removes real social pressure |
| **Using the past as a data source** | retrospective data collection can introduce confounds tied to *when* the data originates | Example 2.10 "Do left-handers die young?": a study found left-handers died younger on average — but this reflected that many left-handed children in the **early 1900s were forced to switch to writing right-handed**, so *older* deceased left-handers were rarer merely as an artefact of historical handedness reporting — not because left-handedness itself shortens life. A **prospective** study following current left/right-handers forward would avoid this confound (but would take far too long practically) |

⚠️ EXAM TRAP: **Confounding vs Interacting — commonly mixed up.**
- **Confounding** = you *can't tell* which variable caused the effect (they're tangled together in your data)
- **Interacting** = you *can* tell, and the answer is "it depends" — effect of one variable genuinely **changes** across levels of another

⚠️ EXAM TRAP: an observational study finding a strong **association/correlation** (e.g. vegetarians have lower heart-attack death rates, even adjusting for smoking/weight/class) does **NOT** prove the vegetarian diet **causes** lower death rates — a possible **confounding variable** could still exist (e.g. general health-consciousness, exercise habits) that wasn't measured/adjusted for.
## A15. Graphing Categorical Data

| Term | Meaning |
|---|---|
| **Frequency** | the **count** of observations falling in a category |
| **Relative frequency** | proportion/percentage of the total in a category (count ÷ total) |
| **Two-way table** (contingency table) | a table of frequencies for **two** categorical variables cross-tabulated together |
| **Row / column percentages** | percentages calculated within each row (row totals to 100%) or within each column (column totals to 100%) of a two-way table |

**Bar charts vs Pie charts**

| | Pie chart | Bar chart |
|---|---|---|
| Number of categorical variables shown | **only 1** at a time | 1 or 2 (compound/clustered/stacked bar charts can show 2) |
| Best for | few categories, showing parts-of-a-whole | comparing category sizes; more categories manageable |
| ⚠️ Must satisfy | percentages/slices must **sum to 100%** | y-axis (scale) **must start at 0**, or comparisons are visually exaggerated |
| Avoid | 3-D or doughnut-hole pie charts (3rd dimension/hole represents nothing, distorts perceived slice size) | 3-D bar charts (same distortion issue) |

- With a two-way table, you must choose whether **row or column percentages** are more meaningful for the question being asked — e.g. "what % of joggers are female" needs one direction, "what % of females jog" needs the other. Decide based on which variable is the natural **explanatory** one.
- **Compound (stacked/clustered) bar charts**: can show up to ~3 categories reasonably; beyond that, becomes hard to read.
- **Ordinal and count data** (few distinct values) use the **same** presentation methods as categorical data (tables, bar charts) — order of categories/bars should follow the natural order for ordinal/count data (not alphabetical).

⚠️ EXAM TRAPS:
- Percentages in a pie chart summing to **>100%** (e.g. 45%+35%+25%=105%) signals an **error** — likely the underlying variable allowed multiple responses per person, or a data/collection mistake (Practice MCQ Q33).
- **3-D charts** where the third dimension carries no real information **distort perceived magnitude** — viewers judge apparent volume/area rather than the true height/value (Practice MCQ Q32).
- **Pictorial graphs** (icons scaled up/down to represent values) mislead because viewers perceive **area** (which scales with the *square* of a linear scale factor), not the underlying value — e.g. doubling a person-icon's height to represent 2× a value makes it look ~4× bigger by area.

## A16. Graphing Continuous Data — the 4 visual displays

| Display | What it shows | Strengths | Weaknesses |
|---|---|---|---|
| **Dotplot** | every individual raw observation as a dot (stacked dots = repeats) | shows every actual data value; simple | "wobbly"/rough appearance; hard to judge shape for large n |
| **Stem-and-leaf plot** | data split into a "stem" (leading digit(s)) and "leaves" (last digit), retaining every value (possibly rounded); leaf unit stated | retains every individual value; good for sorting data by hand | intervals (stems) restricted to lengths of 1, 2, 5, or 10 and must start at multiples of those — limited flexibility; large samples get cluttered |
| **Histogram** | data grouped into equal-width intervals ("bins"); bar **area** ∝ (relative) frequency of that interval | excellent for judging overall **shape** of a distribution; good for large datasets | loses individual values (grouped); ⚠️ choice of starting point & bin width can **substantially change the picture**, especially for small-to-moderate n; a histogram is NOT a bar chart — bars should touch (unlike categorical bar charts) |
| **Boxplot** (box-and-whisker plot) | 5-number summary: min, Q1, median, Q3, max, plus flagged outliers | excellent for **comparing 2+ groups** at a glance; very compact | hides the actual **shape** (can't tell unimodal/bimodal, exact distribution); should **not** be used for small samples (≤10 or so — potentially seriously misleading) |

- All 4 types work for any sample size, though histograms/dotplots need a decent n to show shape reliably, and boxplots for very small n can badly misrepresent the data.
- **6–15 bins/intervals** is generally a good number for a histogram (too few = shape hidden; too many = noisy/sparse).

**Boxplot construction details:**
- Box = Q1 to Q3 (spans the **middle 50%** of data, i.e. the IQR); line inside box = **median**
- Let d = IQR. **Whiskers** extend from the box out to the most extreme data point that is still **within 1.5×d** of the nearest box edge
- Points beyond 1.5×d (but within 3×d) from the box are marked with **\*** (mild outliers)
- Points beyond **3×d** from the box are marked with **○** (extreme outliers), in schemes that distinguish the two
- ⚠️ A boxplot's horizontal width carries **no meaning** — only vertical position matters (for a vertical boxplot)

**Finding the median/quartiles by counting through a stem-and-leaf plot (or any ordered list)** ⭐ — process:
1. Count the **total number of observations**, n (a stem-and-leaf plot lists data already sorted smallest→largest, stem by stem)
2. Median position: if n is **odd**, median = the middle observation, position (n+1)/2 counting from either end. If n is **even**, average the two middle observations, positions n/2 and (n/2)+1
3. To find the median value, count leaves in from the **top** stem downward (or bottom stem upward) — whichever is closer — tallying how many observations you pass, until you reach the target position
4. **Quartiles**: once the median is located, treat the lower half of the data (all observations below the median) as its own ordered list and find **its** median = Q1; do the same with the upper half for Q3. (If n is odd, the median observation itself is excluded from both halves; conventions on this can vary slightly, which is part of why hand and software answers can differ.)
5. Worked example (processing-time stem-and-leaf, n=35): median is the 18th observation (odd n, (35+1)/2=18) — count leaves from the first stem row downward, tallying 3, then 3+8=11, then 11+11=22 — so the 18th observation falls within the 3rd stem row (the row that pushes the cumulative count past 18); read off that specific leaf value and attach it to its stem to get the median.

⚠️ EXAM TRAPS (skewness/shape from a boxplot — heavily tested):
- **Long upper whisker + median close to Q1 (bottom of box)** → distribution is **right-skewed** (positively skewed) — most values bunched low, with a long tail of high values (Practice MCQ Q15, Q36)
- **Long lower whisker + median close to Q3** → distribution is **left-skewed** (negatively skewed)
- For **right-skewed** data: **mean > median** (the long right tail pulls the mean up); the reverse for left-skewed (Practice MCQ Q8, Q37)
- For **symmetric** data: mean ≈ median
- Adding one **extreme high** value to a dataset → the **mean shifts more** than the median (median is resistant/robust; mean is not) (Practice MCQ Q37)
- Larger IQR ⇒ **greater** variability specifically in the **middle 50%** of the data (not the whole dataset) (Practice MCQ Q38)
- IQR (and median) are **more resistant to outliers/extreme skew** than range, variance, or SD, which are all pulled by extreme values (Practice MCQ Q39)

**Histogram-specific exam traps:**
- **Unequal bin widths**: if a histogram uses unequal-width intervals, **bar height can't validly represent frequency directly** — you need **frequency density** (height = frequency ÷ bin width) so that bar **area** (not height) represents the frequency; this is why "frequency density" is used on the y-axis for unequal-width histograms (Practice MCQ Q3, Q35)
- A histogram with **two clear peaks (bimodal)** suggests the data may actually be **two distinct subgroups** mixed together, each with its own typical value (e.g. commute-time histogram peaking at 15min and 60min → likely local vs long-distance commuters) (Practice MCQ Q9)
- **Unimodal** = a single high point/peak in a histogram, dotplot, or stem-and-leaf plot (the "normal"/expected shape for one homogeneous group); contrast with **bimodal** above

## A17. Comparing Groups, Scatterplots & Time Series

**Comparing continuous data across categories** (e.g. handspan by gender):
- ⚠️ Always plot on the **same scale** for valid visual comparison between groups
- **Dotplots and boxplots tend to be most useful** for comparing 2+ groups (compact, side-by-side comparison); histograms/stem-leaf get unwieldy with multiple groups
- **Interaction plot**: plots the **average** of a continuous response variable against one categorical explanatory variable, with **separate lines for each level of a second categorical variable** — if the lines are roughly parallel, no interaction; if lines cross/diverge, the two explanatory variables **interact**

**Scatterplots** (2 continuous variables):
- Response variable conventionally goes on the **y-axis**; explanatory on the **x-axis**
- Purpose: visualise the **relationship** between two continuous (typically) variables
- A third **categorical** variable can be layered in using different symbols/colours per category
- ⚠️ EXAM TRAP: scatterplot x/y **scales should not extend beyond the range of the actual data** — doing so can visually distort/understate the apparent strength of a relationship

**Time series plots**:
- A special case of a scatterplot where **time** is the x-axis variable, used when data are collected repeatedly over time and trends/patterns over time are of interest
- Good for spotting cyclic patterns (e.g. weekly price cycles) or trends

**Dynamic/interactive plots** (e.g. Gapminder): can combine up to 3 continuous variables (x, y, bubble size) + 1 categorical (colour) + time (animation/slider) in one display.

## A18. Outliers

- **Outlier**: a data point that is **not consistent with the bulk of the data**. For a single variable = unusually high/low value. For 2+ variables = an unusual **combination** of values (even if each individual value isn't extreme alone).
- **Extreme values** (the overall min/max in a dataset) are **not automatically outliers** — every dataset has a minimum and maximum by definition; "extreme" just describes position, "outlier" implies inconsistency with the rest of the data's pattern.
- **Do not discard outliers automatically** — first investigate *why* they exist:
  - **Data entry / measurement errors** (e.g. a stopwatch with a dying battery gave a wrong reading; a typo entered "85mm" instead of "58mm") → if the true value can be recovered, **correct** it; if not correctable, it may need to be **discarded**
  - **Legitimate extreme values** (a genuinely unusual but correctly-recorded observation) → should generally be **retained**, since it's real information about the spread of possibilities
  - A subgroup that is systematically different (e.g. a much older/younger student in a mostly-uniform-age class) → decide based on whether that subgroup is relevant to your research question
- Outliers can meaningfully **change** summary statistics (especially mean, SD, range) and can be interesting/informative in their own right, or can obscure the rest of the data if left in a plot (e.g. Example 3.6 — the reef shark's huge weight/length distorted a fish scatterplot so much it had to be excluded from that specific plot to see the other fish clearly, but its data can still be reported/discussed separately).

---

## A19. Worked Example Bank (Topic 1)

*(Concepts extracted from exercise/practice sources — organised by underlying concept, not as copied questions)*

**Observational study vs Experiment — identification pattern:**
| Scenario | Classification | Why |
|---|---|---|
| Comparing GPA of students who live with family vs not (no assignment, just comparing existing groups) | Observational | researcher didn't assign living arrangement |
| Comparing effectiveness of two allergy medications (patients randomly given med A or B) | Randomised experiment | active assignment + randomisation |
| Testing whether servers giving their name gets better tips (servers randomly told to/not to introduce themselves) | Randomised experiment | manipulation + randomisation |
| Whether TV-watching amount is associated with bullying in 6-year-olds (just measuring both) | Observational | no manipulation, ethically can't assign TV/bullying |
| Meditation practice & blood pressure (can't ethically/practically force random people to meditate long-term) | Observational (experiment often impractical) | practical/ethical constraint |
| Vitamin C vs cold prevention, with pill/placebo randomly assigned & investigators (not participants) knowing which is which | Randomised experiment; **single-blind** (only investigators know) | matches single-blind definition exactly |

**Case-control is chosen over an experiment when:** the outcome is rare/serious (e.g. lung cancer), making a forward-looking experiment or cohort study impractically slow or a large ethical concern (can't deliberately expose people to a suspected harmful factor).

**Two-way table row/column percentage practice** (vehicle-type-by-gender / vending-machine style questions): decide direction by the question wording — "what proportion of **females** chose X" → percentage **within the female row/column**; "what proportion of **cold-drink buyers** are female" → percentage within the cold-drink row/column. State clearly which direction you're quoting, since both are "valid," just answering different questions.

**Scatterplot + boxplot combined interpretation** (e.g. grip strength R vs L by hand-dominance): a scatterplot showing a consistent linear cloud across dominance categories (L, R, L/R) suggests the R-vs-L relationship does **not** differ meaningfully by hand dominance; a boxplot of right-grip strength by gender lets you read off **median** (line in box) to compare typical strength between groups directly, without needing the raw data.

---

## A20. ⚠️ Topic 1 — Consolidated Exam Traps

- Variable classification: numbers-as-codes ≠ quantitative; whole numbers ≠ automatically count/discrete; money & time are continuous.
- "Explanatory" and "response" labels do **not** imply proven causation.
- Only a **randomised experiment** can support a causal claim; observational studies show association only.
- **Confounding** (can't separate effects) vs **interacting** (effects genuinely differ by subgroup) — different concepts, often conflated.
- **Selection vs nonresponse vs response bias** — identify by asking: bad *method* (selection), *no answer* from chosen people (nonresponse), or *wrong answer* given (response)?
- **Cluster vs stratified sampling** — stratified = sample from **every** stratum; cluster = sample only **some** whole clusters.
- Bigger sample size ≠ automatically more accurate (Literary Digest vs Gallup) — **randomness of selection** matters far more than raw size.
- Pie chart % must sum to 100%; bar chart y-axis must start at 0; never use 3-D pie/bar charts.
- Unequal histogram bin widths need **frequency density**, not raw frequency, as bar height.
- Boxplot shape reading: long upper whisker/median-near-Q1 = right-skew (mean>median); long lower whisker/median-near-Q3 = left-skew (mean<median).
- Outliers should be investigated before being discarded, not automatically removed.
- Double-blind ≠ single-blind: double = **neither** party knows; single = **only one** party (participant OR researcher) doesn't know.

## A21. Topic 1 — Quick Recall (30-second list)

- PPDAC cycle: Issues → Planning → Collecting → Exploring/Analysing → Interpreting (cyclical)
- 3 investigation types: Observational / Experiment / Survey — only Experiment (randomised) shows causation
- Variable tree: Categorical(Nominal/Ordinal) vs Quantitative(Discrete-count/Continuous[Interval/Ratio])
- 3 sampling methods beyond SRS: Stratified (every stratum sampled) / Cluster (some clusters sampled) / Multistage (combo)
- 3 bias types: Selection / Nonresponse / Response
- Margin of error ≈ 1/√n; n=1500 → ≈±3%; quadrupling n halves MoE
- Blinding: single (1 party unaware) vs double (both parties unaware)
- 4 continuous-data displays: Dotplot / Stem-leaf / Histogram / Boxplot — boxplot best for comparing groups, histogram best for shape
- Boxplot skew reading: long tail side = skew direction; mean follows the tail
- Confounding = tangled, can't separate; Interacting = separable, effect depends on the other variable; Lurking = unmeasured variable causing a spurious link
# PART B — TOPIC 2: Summarising Data & Probability

## B1. Measures of Location

Chapter 4 focuses on 3 features of quantitative data: **location** (centre), **spread/dispersion**, and **shape**.

| Measure | Definition | Resistant to outliers? |
|---|---|---|
| **Mean (x̄)** | arithmetic average: Σx/n | ❌ No — pulled toward outliers/skew |
| **Median** | middle value when sorted (avg of 2 middle values if n even) | ✅ Yes — robust |
| **Mode** | most frequently occurring value/category | ✅ Yes (not affected by extreme values at all) |

- **Mode is the appropriate measure for the "most common" category of a nominal/categorical variable** — mean and median don't apply to nominal data (Practice MCQ Q12: most common coffee type ordered → mode).
- **Median is generally least affected by an unusually large/small value** (e.g. one huge executive salary in a company) — the standard "resistant" measure of centre (Practice MCQ Q10).
- Worked example (58 fish lengths incl. one 1200mm reef shark outlier): removing the outlier changed the **mean** substantially (278.8mm → 262.67mm) but changed the **median** only slightly (255mm → 250mm) — direct demonstration of the mean's outlier-sensitivity vs the median's robustness.
- Median calculation (n=8, even): sort ascending, average the 4th and 5th values.

## B2. Measures of Spread / Dispersion

Three commonly used measures:

| Measure | Formula | What it captures |
|---|---|---|
| **Range** | Max − Min | crude total spread; very outlier-sensitive |
| **Interquartile range (IQR)** | Q3 − Q1 | spread of the **middle 50%**; outlier-resistant |
| **Sample standard deviation** | √(sample variance); calculated via Excel in this course | "typical"/average distance of observations from the mean |

**Quartile calculation, "by hand"** (illustrated on fish-length data, n=57 after excluding the reef shark outlier):
- Median (Q2) = 29th observation (of 57, sorted) = 250
- Lower half = observations 1–28 (below median); Q1 = "middle" of these 28 → halfway between the 14th and 15th observations
- Upper half similarly gives Q3 (halfway between two values, ended up 297.5 in the worked example)
- IQR = Q3 − Q1 = 297.5 − 237.5 = 60mm; Range = 410 − 85 = 325mm
- ⚠️ CONFLICT: the hand method above and Excel's `QUARTILE`/`PERCENTILE` functions can give **slightly different numeric answers** — this is expected and not an error; different valid algorithms exist.

**Quartile/percentile interpretation table** (worked "customer spending" example):
| Statistic | Interpretation |
|---|---|
| Q1 = value V | 25% of observations are below V, 75% are above |
| Median = value V | 50% below, 50% above |
| Q3 = value V | 75% below, 25% above |
| IQR = Q3−Q1 | the middle 50% of observations span this range |

- Quoting the **5-number summary** (min, Q1, median, Q3, max) together, e.g. as "typical value is about $4,000, but the middle 50% of customers spend between $2,000 and $7,000," is **more informative** than quoting the mean alone.
- **Percentiles** generalise quartiles to any %; e.g. growth charts show a child's height/weight percentile relative to peers.

**Variance & standard deviation**
- **Sample variance**: "on average, how far are observations spread from the mean?" (squared units)
- **Sample SD**: √variance — same units as the original data, so more interpretable (e.g. salary variance is in $², but SD is in $)
- Course convention: SD/variance are **calculated via Excel only** in this course (`=STDEV(range)` / `=STDEV.S(range)`), not hand-derived from the raw definitional formula
- ⚠️ EXAM TRAPS:
  - **Variance's units are always the square of the original variable's units** (e.g. advertising spend in $ → variance in $²) (Practice MCQ Q28)
  - **If every value in a dataset is identical, SD = 0** (Practice MCQ Q29)
  - **Adding the same constant to every value in a dataset does NOT change the SD** (it shifts the mean, but the *spread* is unchanged) (Practice MCQ Q20)
  - Comparing two datasets/groups by SD: the one with the **higher SD has more variability** (e.g., in investment risk, Portfolio B at 11% SD has more variability/risk than Portfolio A at 4% SD — SD says nothing directly about which has the *higher expected return*, only about variability) (Practice MCQ Q30)
  - Interpreting a stated SD (e.g. "mean travel time 35 min, SD 6 min"): means observations **typically vary around the mean by about 6 minutes** — NOT that every observation is exactly 6 minutes from the mean, and NOT the average value itself (Practice MCQ Q40)

## B3. Shape & Skewness

- **Symmetric data**: roughly mirror-image left and right of the centre; **mean ≈ median**
- **Right-skewed** (positively skewed): long tail toward **high** values → **mean > median** (the tail pulls the mean up but the median resists)
- **Left-skewed** (negatively skewed): long tail toward **low** values → **mean < median**
- **Histograms are the standard tool to assess skewness visually**
- Symmetric data is *preferred* in practice — easier to summarise with mean & SD, and easier to compare across groups
- **Transformations** (e.g. a **logarithmic** transformation) can sometimes convert skewed (asymmetric) data into roughly symmetric data, making it easier to analyse (worked example: monthly phone bills, right-skewed, became more symmetric after a log transform)
- Worked example: customer arrival-gap times at a coffee shop — most gaps are short (1–3 min) with occasional long gaps (15–20 min) when quiet → **right-skewed**, long right tail → the **median gives a better "typical" picture** than the mean, because a few long gaps pull the mean upward

⚠️ EXAM TRAP (bimodal ≠ normal): a histogram with **two distinct peaks** does not follow a standard bell-shaped/Normal pattern — it suggests **two separate subgroups** are mixed in the data, each with a different typical value (Practice MCQ Q9, restated from Topic 1's histogram section — applies equally here).

## B4. ⭐ Bell-Shaped Distributions & The Empirical Rule

- **Bell-shaped distribution**: data cluster around the centre, with progressively fewer observations further from the centre in either direction; arises naturally when an outcome depends on **many small, roughly independent factors** that mostly cancel out (e.g. employee productivity depends on many small positive/negative factors — most days land near "typical," very high or very low productivity requires many factors aligning in the same direction simultaneously, which is rare)

**The Empirical Rule** (only valid for bell-shaped/Normal-like data):
| Range | % of values |
|---|---|
| μ ± 1σ | **68%** |
| μ ± 2σ | **95%** |
| μ ± 3σ | **99.7%** |

- μ = mean ("mu"), σ = standard deviation ("sigma")
- **Tail shortcut**: outside ±2σ leaves 5% total → 2.5% in *each* tail; outside ±3σ leaves 0.3% total → 0.15% in *each* tail (assumes symmetry, standard for bell-shaped data)
- Use the Empirical Rule to judge whether a specific value is "**unusual**": a value beyond 2σ from the mean is fairly unusual (~2.5% in that direction); beyond 3σ is very unusual (~0.15%) — useful for quality control (spotting defective/out-of-spec products) or flagging atypical individual measurements

**Worked examples:**
- Sleep hours: μ=7, σ=1.7 → 68% of students sleep 5.3–8.7 hrs; 95% sleep 3.6–10.4 hrs; 99.7% sleep 1.9–12.1 hrs
- Component diameter: μ=50mm, σ=2mm → 46mm is 2σ below mean (only ~2.5% of parts smaller); 56mm is 3σ above mean (only ~0.15% of parts larger) → useful for a manufacturer spotting quality-control problems
- Head circumference: μ=56cm, σ=2cm → 52cm is (56−52)/2 = **2σ below** mean (~2.5% of men smaller); 62cm is (62−56)/2 = **3σ above** mean (~0.15% of men bigger)

**Empirical estimation from real data (Lab 2)** — checking whether the Empirical Rule's *predicted* % actually matches the *observed* % in a real dataset:
1. Compute sample mean & SD (`=AVERAGE`, `=STDEV`)
2. Compute μ±σ, μ±2σ, μ±3σ boundary values
3. Use `=COUNTIFS(range,">lower",range,"<upper")` to count how many actual observations fall inside each boundary pair
4. Divide each count by n to get the **observed** proportion, and compare to the **68%/95%/99.7%** predicted by the rule
5. Close agreement supports treating the distribution as bell-shaped; a big mismatch suggests it isn't

⚠️ EXAM TRAP: the Empirical Rule **only applies to bell-shaped (roughly Normal/symmetric) data** — applying it to a skewed distribution gives **wrong** estimates; always sanity-check shape (e.g. via histogram) first.

## B5. Boxplots Revisited — Five-Number Summary

- Boxplot = the "**five-number summary**" displayed graphically: Minimum, Q1 (bottom of box), Median (line inside box), Q3 (top of box), Maximum (via whiskers, subject to the 1.5×IQR outlier rule from A16)
- "Box" = middle 50% of data; whiskers extend toward min/max (bounded by the 1.5×IQR rule); outliers plotted separately as points/asterisks
- Comparing boxplots side-by-side (e.g. by gender) lets you read off and compare **medians directly** (e.g., median right-hand grip strength: males vs females) without raw data
## B6. Probability Basics & Rules

- **Probability**: a measure of the **likeliness** of an event occurring
- Core restrictions (all probabilities must satisfy):
  - **Non-negative**: P(event) ≥ 0
  - **Cannot exceed 1**: P(event) ≤ 1
  - **All outcomes together = 1**: the probability of "all possible situations together" is exactly 1 (certainty)
- **Core rules**:
  - **Complement rule**: P(event) = 1 − P(event does NOT happen) → e.g. P(rain) = 1 − P(no rain)
  - **"At least one" rule**: P(at least one occurs) = 1 − P(none occur) → e.g. P(rain on ≥1 day next week) = 1 − P(no rain all week)
- **Probability model**: a systematic way of assigning probabilities, either estimated **from data** (empirically) or from **assumptions** about the situation (theoretically)

**Addition rule (overlapping categories)** ⭐ — needed whenever two events/categories can both apply to the same subject:
- P(A or B) = P(A) + P(B) − P(A **and** B) — subtract the overlap once so it isn't double-counted
- P(A but **not** B) = P(A) − P(A **and** B)
- Worked example (car safety checks, n=200): 50 defective brakes, 115 defective steering, 45 defective **both**:
  - P(defective brakes) = 50/200 = 0.25
  - P(defective steering) = 115/200 = 0.575
  - P(both defects) = 45/200 = 0.225
  - P(brakes but **not** steering) = (50−45)/200 = 5/200 = 0.025 ⚠️ NOT simply 50/200 — must subtract the 45 that overlap with steering first
  - (P(brakes or steering) would be 0.25+0.575−0.225 = 0.60 by the addition rule)
- ⚠️ EXAM TRAP: "A but not B" is **not** the same as "A" — always subtract the "both" overlap first when two categories can co-occur for the same subject.

**Where probability estimates come from — empirical estimation:**
- Formula: estimated P(event) = (number of times event occurred) ÷ (total number of trials/observations)
- Simple examples: 10 of 50 shoppers buy brand X → P(buys X) ≈ 10/50 = 0.20; 3 of 20 CDs defective → P(defective) ≈ 3/20 = 0.15
- From a two-way (contingency) table: P(event) = (relevant cell or row/column count) ÷ (relevant total) — e.g. P(morning commuter uses lift) = 222/432 = 0.514
- From continuous data: probabilities are estimated for **intervals** (ranges), e.g. P(pizza order takes <15 min) = (count of orders under 15 min)/(total orders)
- 120 deliveries, 18 late → estimated P(late) = 18/120 = **0.15** (Practice MCQ Q25)

## B7. Random Variables: Discrete vs Continuous ⭐

| | **Discrete random variable** | **Continuous random variable** |
|---|---|---|
| Takes values from | a list of **distinct, separate** values | any value within an **interval** |
| Probabilities assigned to | **exact values** | **intervals** (never to one exact point) |
| Described by | a **probability function** (table/formula mapping each value to its probability) | a **probability density function (pdf)** — a curve |
| Probability = | the value looked up in the probability function/table | the **area under the pdf curve** over the relevant interval |
| Example | number of defective bulbs in a pack of 6 | time waiting for a bus |

**Discrete random variable rules** (same core rules as general probability, restated for a variable X):
- Σ [all P(X=x)] = **1** (probabilities over all possible values sum to 1)
- Each individual P(X=x) is between 0 and 1
- Worked example (defective bulbs, X = number defective, values 0–5): compute P(X=3) by reading the table directly; compute P(X<3) by **summing** P(X=0)+P(X=1)+P(X=2)

**Continuous random variable / pdf**:
- The pdf is a curve; probability for an interval = **area under the curve** over that interval (never P(X = a single exact point), which is technically 0 for continuous variables)
- **Uniform random variable**: equally likely to take any value in an interval — pdf is a flat/rectangular shape
- Worked example (bus arrives every 10 min, waiting time X uniform on 0–10): P(5<X<7) = area of a rectangle = length × height = 2 × 0.10 = **0.20**

⚠️ EXAM TRAP: **Medians and quartiles are much easier to define for continuous random variables** (median = the point with 0.5 probability on either side) **than for discrete random variables**, because discrete probability comes in "lumps" rather than a smooth continuum — a discrete distribution's exact 50th-percentile point may not exist cleanly.

## B8. Expected Value, Variance & SD of a Random Variable

- **Expected value** E(X) (also called the **mean** of the random variable, μ): the **long-run average** value if the random process were repeated many, many times
- Formula: E(X) = Σ [x · P(x)] — multiply each possible outcome by its probability, sum them all
- ⚠️ EXAM TRAP: E(X) is a **long-run average across many repetitions**, NOT a promise/guarantee about any single trial, and it need **not even be one of the possible outcome values** (e.g. E(X)=1.9 orders/hour, but you can never literally receive 1.9 orders) (Practice MCQ Q21)
- Worked examples:
  - Orders/hour probability table (0:0.10, 1:0.20, 2:0.40, 3:0.30) → E(X) = 0(.10)+1(.20)+2(.40)+3(.30) = **1.9**
  - Investment returns ($20k:0.5, $50k:0.3, −$10k:0.2) → E(X) = 10,000+15,000−2,000 = **$23,000**
  - Insurance claim (No claim 90%:$0, Minor 8%:$5,000, Major 2%:$20,000) → E(X) = 0+400+400 = **$800** average claim cost per customer, over many customers
- **Variance of a random variable**: Var(X) or σ² — measures spread of the probability distribution
- **SD of a random variable**: σ = √Var(X), same units as X ("sigma"); σ² read aloud as "**sigma squared**"

**Median & quartiles of a probability distribution:**
- **Median**: the value with **exactly 0.5 probability on either side** — e.g. bus-waiting median = 5 min means P(wait<5min)=0.5
- **Lower quartile**: the value with **25% probability below** it (e.g. bus-wait lower quartile = 2.5 min)
- **Upper quartile**: the value with **75% probability below** it (e.g. bus-wait upper quartile = 7.5 min)
- Worked example (loan approval time, median = 3 days): means 50% of applications are approved within 3 days, 50% take longer; useful because a few very slow outlier applications (20–30 days) would pull the **mean** upward, so the median gives a more typical/representative picture — same logic as B3's skewness discussion

## B9. Parameters vs Estimates

| Term | Meaning |
|---|---|
| **Parameter** | a characteristic of a random variable / the whole population (e.g. the true population mean, median, variance) — usually unknown |
| **Estimate** (= "descriptive statistic") | a value **calculated from a sample** of data, used to estimate the corresponding unknown parameter |

- Example: wanting the mean height of ALL Australian 18-year-old females (parameter, unknown) → take a sample of 100 such females, compute the **sample mean** height → use that as the **estimate** of the parameter
- Example: sample mean walking time = 10.2 min is an *estimate* of the population mean parameter; sample median = 9.85 min estimates the population median parameter

---

## B10. ⭐ Misleading Graphs — comprehensive reference

| Type of misleading graph | What goes wrong | How to spot / avoid |
|---|---|---|
| **Errors** | e.g. pie-chart percentages that don't sum to 100% | check the percentages actually total 100% |
| **3-D graphs** | an added third dimension carries no real data, but visually distorts comparisons | avoid 3-D bar/pie charts entirely; use plain 2-D |
| **Pictorial / icon graphs** | icons/images scaled up to represent bigger values — the eye reads **area or volume**, not just height/length, exaggerating the visual difference (e.g. a person-icon scaled 5× taller for a 5× larger salary looks vastly more than 5× bigger by apparent area) | be suspicious of any graph using resized pictures instead of plain bars/points |
| **Graphing only part of the data** | omitting data points (e.g. showing only half a year of temperature data) can "support almost any position desired" | check whether the full relevant time range / dataset is shown, and whether omissions are disclosed |
| **Scatterplot axes extending beyond the data range** | can visually distort the apparent strength/shape of a relationship | axes should be scaled to the actual data range |
| **Axis scale not starting at zero** (bar charts) | makes small real differences look dramatic (e.g. Store A $98,000 vs Store B $100,000 looks huge if the axis starts at $97,000) | bar chart y-axis (representing counts/frequencies/amounts by bar height) **must start at 0**; check the axis start value before interpreting a bar chart's visual message |
| **Using graphs to imply causation** | a strong association shown in a scatterplot/graph does **not** prove one variable causes the other — a lurking/confounding variable may be responsible | e.g. breastfeeding rates vs infant death rates showed a strong positive relationship across countries — but this did **not** mean breastfeeding causes death; the real driver was a lurking variable, **sanitation level**, which happened to be negatively associated with breastfeeding rates in that dataset |
| **Unequal histogram bin widths without frequency density** | see A16 — bar height alone misrepresents frequency when bins differ in width | use frequency density (height = frequency/width) so **area** represents frequency |

⚠️ EXAM TRAPS (misleading graphs, from Practice MCQs):
- Bar chart axis starting above zero → **exaggerates the visual difference** between values, even though the actual numeric difference is small (Practice MCQ Q31)
- 3-D chart with values shown as differently-sized 3-D blocks → the **visual volume/area may exaggerate differences** beyond what the numbers actually show (Practice MCQ Q32)
- Pie chart categories summing above 100% → data/category **error** (categories may overlap, or the question allowed multiple answers) (Practice MCQ Q33)
- Standard histogram bars (equal width) **should touch** — gaps are for categorical bar charts, not histograms of numerical/continuous data (Practice MCQ Q34)
- Real example: a graph comparing "% on welfare (counted per household)" vs "% with a full-time job (counted per individual)" used **inconsistent counting units** between the two bars being compared — a subtler form of misleading graph (mismatched denominators/definitions, not just an axis trick)
- Real example: claiming "fewer male teachers caused more youth crime" from a graph with a non-zero-started axis — re-drawing with axis starting at 0 showed the trends were **far less dramatic**, and even then, correlation over time doesn't establish that cause (a **lurking variable**, e.g. broader social change over time, could explain both trends)

---

## B11. Worked Example Bank (Topic 2)

**Probability from a two-way (contingency) table:**
| | Lift | Stairs | Total |
|---|---|---|---|
| Morning | 222 | 210 | 432 |
| Off-peak | 232 | 130 | 362 |
| Evening | 300 | 213 | 513 |
| **Total** | 754 | 553 | 1307 |

P(morning commuter uses lift) = 222/432 = 0.514; P(evening commuter uses lift) = 300/513 = 0.585 — shows how to extract a **conditional-style** probability (within one row) from a contingency table.

**Discrete probability table sanity check**: probabilities must sum to 1 — e.g. given P(X=0)=0.95, P(1)=0.03, P(2)=?, P(3)=0.003, P(4)=0.0015, P(5)=0.0005, solve for the missing value by subtracting the known probabilities from 1.

**Continuous/uniform pdf worked example**: bus arrives every 10 minutes (X ~ Uniform(0,10)); P(5<X<7) = rectangle area = 2 min × (1/10 per min height) = 0.20; median = 5 min (50% chance of arriving within 5 min); lower quartile = 2.5 min, upper quartile = 7.5 min.

---

## B12. ⚠️ Topic 2 — Consolidated Exam Traps

- Mode → best "average" for nominal/categorical data (not mean/median).
- Median is resistant to outliers; mean is not — an extreme value shifts the mean far more.
- Variance units = squared units of the original variable; SD units = original units.
- Adding a constant to all values doesn't change SD (only shifts the mean); identical values → SD=0.
- Empirical Rule (68/95/99.7) applies **only** to bell-shaped/Normal-like data — check shape first (histogram).
- Tail % shortcut: outside ±2σ → 2.5% per tail; outside ±3σ → 0.15% per tail.
- Right-skew → mean > median (long tail pulls mean up); left-skew → mean < median.
- E(X) is a long-run average, not a per-trial guarantee, and need not be a possible value of X.
- Discrete RV: probabilities on exact values, sum to 1. Continuous RV: probabilities on intervals via area under a pdf curve; P(X=exact point)=0.
- Median/quartiles are well-defined and easy for continuous RVs, "lumpy"/awkward for discrete RVs.
- Parameter = true population value (usually unknown); estimate/statistic = value computed from a sample.
- Misleading graphs: non-zero bar-chart axis, 3-D distortion, pictorial/icon area distortion, unequal histogram bins without frequency density, partial data, causation implied from correlation (check for lurking variables).

## B13. Topic 2 — Quick Recall (30-second list)

- 3 spread measures: Range (crude) / IQR=Q3−Q1 (resistant) / SD (typical distance from mean, via Excel)
- Mean sensitive to outliers & skew; Median & Mode resistant
- Right-skew: mean>median, long right tail. Left-skew: mean<median, long left tail.
- Empirical Rule: 68% within 1σ / 95% within 2σ / 99.7% within 3σ — bell-shaped data ONLY
- Probability: 0≤P≤1, ΣP=1; Complement P(not A)=1−P(A); At-least-one = 1−P(none)
- Discrete RV → probability function, exact values, ΣP=1. Continuous RV → pdf, area=probability, intervals only
- E(X)=Σx·P(x) = long-run average, not a guarantee
- Var(X)=σ²; SD=σ=√Var(X)
- Median of a distribution = 50%-point; quartiles = 25%/75%-points
- Parameter (population, unknown) vs Estimate/statistic (sample, calculated)
- Misleading graphs: zero-axis rule for bar charts; no 3-D; pie % must total 100%; histogram bars touch; unequal bins need frequency density; correlation ≠ causation (watch for lurking variables)
# PART C — "What to Look For" Keyword Map

Use this to map exam-question wording straight to the right section.

| If the question mentions… | → Think |
|---|---|
| "no natural order," category **names** | Nominal categorical (A3) |
| rating scale, rank, level of agreement, Likert | Ordinal categorical (A3) |
| "number of…", count of events/items/people | Discrete (count) quantitative (A3) |
| measured to the nearest cm/kg/sec/$, decimal places possible | Continuous quantitative (A3) |
| "true zero," ratios meaningful vs "zero doesn't mean none" | Ratio vs Interval data (A3) |
| investigator **didn't** control/manipulate anything | Observational study (A4) |
| investigator **assigned** treatments/conditions | Experiment (A4) |
| asking people questions directly | Survey (A4) |
| "outcome of interest," "what we're measuring the effect on" | Response variable (A5) |
| "chosen to explain/predict," "manipulated" | Explanatory variable (A5) |
| population, generalise to a larger group | Sampling / representativeness (A7) |
| every unit equal chance, "drawing from a hat" | Simple random sample (A7) |
| subgroups by region/gender sampled separately, each fully sampled | Stratified sampling (A8) |
| only some whole groups/areas visited (no full individual list) | Cluster sampling (A8) |
| sample chosen doesn't represent the population, by design/method | Selection bias (A9) |
| a subgroup fails to respond/can't be reached | Nonresponse bias (A9) |
| people give untrue/dishonest answers, sensitive topic, leading wording | Response bias (A9) |
| "±3%," "how big should my sample be" | Margin of error (A10) |
| "1 in 2 Australians…" loaded phrasing before a question | Deliberate/leading bias in question wording (A11) |
| two treatments/conditions, groups treated identically except treatment | Control group (A12) |
| fake pill, "no active ingredient" | Placebo (A12) |
| "neither participant nor researcher knows" | Double-blind (A12) |
| "only the researcher (or only the participant) doesn't know" | Single-blind (A12) |
| same subjects matched by trait, each pair split between treatments | Matched-pair / block design (A12) |
| rare disease/outcome, compare "cases" to similar people without it | Case-control study (A13) |
| "can't separate the effects of two variables" | Confounding variable (A14) |
| "effect of A depends on the level of B" | Interacting variables (A14) |
| "behaved differently just from being watched" | Hawthorne effect (A14) |
| "researcher unintentionally treated groups differently" | Experimenter effect (A14) |
| does the lab result generalise to real life? | Ecological validity / generalisability (A14) |
| single categorical variable, parts of a whole | Pie chart (A15) |
| comparing category counts, 1–2 categorical variables | Bar chart (A15) |
| two categorical variables cross-tabulated | Two-way (contingency) table, row/column % (A15) |
| judging overall shape/distribution of numeric data | Histogram (A16) |
| comparing 2+ groups' spread & centre compactly | Boxplot (A16) |
| every raw data point retained, sorted by digit | Stem-and-leaf (A16) |
| "long upper whisker," "median near bottom of box" | Right-skew, mean > median (A16, B3) |
| "long lower whisker," "median near top of box" | Left-skew, mean < median (A16, B3) |
| two peaks in a histogram | Two subgroups mixed (bimodal), not Normal (A16, B3) |
| relationship between 2 continuous variables | Scatterplot (A17) |
| data collected repeatedly over time | Time series plot (A17) |
| point "not consistent with the rest of the data" | Outlier — investigate before removing (A18) |
| "most common," nominal data centre | Mode (B1) |
| "resistant to outliers," "typical value despite extreme cases" | Median (B1, B2) |
| "spread of the middle 50%" | IQR (B2) |
| "average distance from the mean," Excel-calculated spread | Standard deviation (B2) |
| squared units in the answer | Variance (B2) |
| "bell-shaped," "Normal," "68%/95%/99.7%" | Empirical Rule (B4) |
| "how unusual is this value," "how many SDs away" | Empirical Rule tail % (B4) |
| P(A or not A), "certain event" | Probability sums to 1 / complement rule (B6) |
| "estimate from observed data," count/total | Empirical probability (B6) |
| "exact values," "list of distinct outcomes" | Discrete random variable (B7) |
| "any value in an interval," "area under a curve" | Continuous random variable / pdf (B7) |
| "long-run average," "over many repetitions" | Expected value E(X) (B8) |
| "value with 50% probability either side" | Median of a distribution (B8) |
| "true population value" vs "value from my sample" | Parameter vs Estimate (B9) |
| bar chart axis doesn't start at 0 | Misleading graph — exaggerated differences (B10) |
| chart uses 3-D blocks or resized pictures/icons | Misleading graph — area/volume distortion (B10) |
| pie chart percentages don't add to 100% | Misleading/erroneous graph (B10) |
| strong relationship shown ⇒ claim of causation | Correlation ≠ causation; check for a lurking variable (B10) |
| unequal-width histogram bins | Need frequency density, not raw height (A16, B10) |
| "=AVERAGE / =MEDIAN / =QUARTILE / =STDEV / =COUNTIF(S)" | Excel function reference (Formula Sheet) |
| "$" before a cell reference (e.g. `$B$2`) | Absolute cell reference (Formula Sheet) |

---

# PART D — ⚡ EXAM-DAY RAPID REFERENCE
*(30-second emergency revision page — highest-value content only)*

### Must-Know Definitions
- **Statistics** = the science of learning from data; study of variation & uncertainty.
- **PPDAC cycle**: Issues → Planning → Collecting → Exploring/Analysing → Interpreting (cyclic).
- **Population** = whole group of interest; **Sample** = subset actually measured; **Census** = sample = population.
- **Response variable** = outcome measured; **Explanatory variable** = possible cause/predictor.
- **Confounding** = can't separate two variables' effects; **Interacting** = effect of one genuinely depends on the other; **Lurking** = an unmeasured variable driving a spurious relationship.
- **Parameter** = true population value (unknown); **Estimate/statistic** = value computed from a sample.
- **Outlier** = inconsistent with the bulk of data (not just "the min/max").

### Must-Know Formulas
- Margin of error ≈ **1/√n** (n=1500 → ≈±3%; 4×n halves the margin of error)
- IQR = **Q3 − Q1**
- Empirical Rule: **68% (±1σ) / 95% (±2σ) / 99.7% (±3σ)** — bell-shaped data only; tails outside ±2σ ≈2.5% each, outside ±3σ ≈0.15% each
- Complement rule: **P(not A) = 1 − P(A)**; At-least-one: **P(≥1) = 1 − P(none)**
- Empirical probability = **count ÷ total**
- Expected value: **E(X) = Σ x·P(x)** — long-run average, not a per-trial guarantee
- Variance units = **original units²**; SD units = original units
- Excel: `AVERAGE`, `MEDIAN`, `QUARTILE(range,1/2/3)`, `STDEV`/`STDEV.S`, `COUNTIF`, `COUNTIFS`; `$` = absolute reference

### Must-Know Rules
- Only a **randomised experiment** can support a cause-and-effect claim.
- Bigger sample ≠ more accurate — **randomness of selection** is what matters (Literary Digest vs Gallup).
- Adding a constant to every value: **mean shifts, SD unchanged**.
- Right-skew → **mean > median**; Left-skew → **mean < median**; Symmetric → mean ≈ median.
- Bar chart axis **must start at 0**; pie chart % **must sum to 100%**; never use 3-D charts.
- Histogram bars **touch** (unlike categorical bar charts); unequal bin widths need **frequency density**.

### Easily Confused Concepts
- **Stratified** (every stratum sampled) vs **Cluster** (only some whole clusters sampled)
- **Selection bias** (bad sampling method) vs **Nonresponse bias** (chosen people don't answer) vs **Response bias** (people answer untruthfully)
- **Single-blind** (one party unaware) vs **Double-blind** (neither party aware)
- **Confounding** (can't separate) vs **Interacting** (effect depends on the other variable, but separable)
- **Nominal** (no order) vs **Ordinal** (ordered categories)
- **Discrete/count** (exact countable values) vs **Continuous** (any value in an interval)
- **Interval data** (no true zero, e.g. °C) vs **Ratio data** (true zero, e.g. length)
- **Mean** (sensitive to outliers/skew) vs **Median** (resistant) vs **Mode** (for nominal data)
- **Range** (crude, 2 points only) vs **IQR** (resistant, middle 50%) vs **SD** (typical distance from mean)
- **Discrete random variable** (probability functions, exact values) vs **Continuous random variable** (pdf, area = probability over an interval)
- **Retrospective study** (past data/recall) vs **Prospective study** (followed forward in time)
- **Extreme value** (just the min/max) vs **Outlier** (inconsistent with the rest of the data)

### Important Exceptions
- Whole-number data is **not** automatically count/discrete data (e.g. age in whole years is still continuous).
- Numbers that are really codes (e.g. 1=Male, 2=Female; student IDs) are **not** quantitative.
- "Explanatory" and "response" labels do **not** by themselves prove causation.
- The Empirical Rule **only** applies to bell-shaped/Normal-like distributions.
- Quartile "by-hand" answers may legitimately differ slightly from Excel's `QUARTILE`/`PERCENTILE` output.

### Biggest Exam Traps
- Confusing skew direction: **long tail = skew direction = where the mean gets pulled toward**.
- Treating a **Likert-scale number** (e.g. satisfaction 1–5) as quantitative — it's **ordinal categorical**.
- Assuming a large sample is automatically a good/unbiased sample.
- Assuming an association/correlation proves causation — always consider a **lurking/confounding variable**.
- Forgetting **frequency density** for unequal histogram bins.
- Reading a bar chart's visual gap without checking whether the **axis starts at zero**.
- Treating **E(X)** as a guaranteed single-trial outcome rather than a long-run average.

### MCQ Patterns
- If a scenario has **no manipulation, just comparison of existing groups** → observational study, **cannot** conclude causation.
- If a scenario has **random assignment to conditions** → randomised experiment, causation **can** be inferred (with due care re: confounds/blinding).
- If asked "which measure is least affected by an outlier" → **median** (or IQR for spread, or mode for categorical centre).
- If asked "why might this histogram/bar chart mislead" → check axis start point, bin widths, and dimensionality (3-D) first.
- If given a probability table and asked for a "long-run average" or "expected" value → compute **Σx·P(x)**.
- If told a distribution is "bell-shaped" and given μ and σ → apply the **Empirical Rule** (68/95/99.7) directly.
- If comparing two groups' **spread** → whichever has the **larger SD/IQR/range has more variability**.
